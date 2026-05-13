#include <cglm/cglm.h>

#include "engineCore.h"
#include "state.h"

#include "defaultCamera.h"
#include "model.h"
#include "texture.h"
#include "sound.h"
#include "obj.h"
#include "font.h"
#include "gltf.h"
#include "rectangle.h"

#include "entity.h"
#include "objBuilder.h"
#include "fontBuilder.h"

#include "renderPassCore.h"

#include "graphicsPipelineObj.h"
#include "graphicsPipelineLayout.h"
#include "descriptorSetLayoutObj.h"

static void addTextures(struct EngineCore *this) {
    struct ResourceManager *textureManager = calloc(1, sizeof(struct ResourceManager));

    addResource(textureManager, TEXTURE_COLOR, loadTextures(&this->graphics, 4, (struct TextureData[]){
        { .data = "textures/hex_small.png" },
        { .data = "textures/player1.png" },
        { .data = "textures/hex_cracked.png" },
        { .data = "textures/floor2.png" },
    }), unloadTextures);
    addResource(textureManager, TEXTURE_CUBEMAP, loadCubeMaps(&this->graphics, (const char *[]) {
        "textures/CubeMap_purple/xpos.png",
        "textures/CubeMap_purple/xneg.png",
        "textures/CubeMap_purple/ypos.png",
        "textures/CubeMap_purple/yneg.png",
        "textures/CubeMap_purple/zpos.png",
        "textures/CubeMap_purple/zneg.png",
    }), unloadTextures);

    addResource(&this->resource, TEXTURES, textureManager, cleanupResourceManager);
}

static void addModelData(struct EngineCore *this) {
    struct ResourceManager *modelData = calloc(1, sizeof(struct ResourceManager));

    addResource(modelData, MODEL_PLAYER, loadModel("models/player.glb", &this->graphics), destroyActualModel);
    addResource(modelData, MODEL_HEX, loadModel("models/hex.glb", &this->graphics), destroyActualModel);
    addResource(modelData, MODEL_FONT, loadModel("fonts/c.ttf", &this->graphics), destroyActualModel);
    addResource(modelData, MODEL_FLAT, loadModel("rectangle.rec", &this->graphics), destroyActualModel);
    addResource(modelData, MODEL_SKYBOX, loadModel("models/my_skybox.obj", &this->graphics), destroyActualModel);

    addResource(&this->resource, MODELS, modelData, cleanupResourceManager);
}

static void addRenderPassCoreData(struct EngineCore *this) {
    struct ResourceManager *renderPassCoreData = calloc(1, sizeof(struct ResourceManager));

    addResource(renderPassCoreData, RENDER_PASS_CLEAN, createRenderPassCore((struct renderPassCoreBuilder) {
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .initLayout = VK_IMAGE_LAYOUT_UNDEFINED
    }, &this->graphics), freeRenderPassCore);
    addResource(renderPassCoreData, RENDER_PASS_STAY, createRenderPassCore((struct renderPassCoreBuilder) {
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .initLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    }, &this->graphics), freeRenderPassCore);

    addResource(&this->resource, RENDER_PASS_CORE, renderPassCoreData, cleanupResourceManager);
}

static void addObjectLayout(struct EngineCore *this) {
    struct ResourceManager *objectLayoutData = calloc(1, sizeof(struct ResourceManager));

    addResource(objectLayoutData, OBJECT_LAYOUT_OBJ,
        defaultObjDescriptorSetLayout(this->graphics.device),
        destroyDescriptorSetLayout
    );
    addResource(objectLayoutData, OBJECT_LAYOUT_GLTF,
        defaultGltfDescriptorSetLayout(this->graphics.device),
        destroyDescriptorSetLayout
    );
    addResource(objectLayoutData, OBJECT_LAYOUT_REC,
        defaultRecDescriptorSetLayout(this->graphics.device),
        destroyDescriptorSetLayout
    );
    addResource(objectLayoutData, OBJECT_LAYOUT_FONT,
        defaultFontDescriptorSetLayout(this->graphics.device),
        destroyDescriptorSetLayout
    );
    addResource(objectLayoutData, OBJECT_LAYOUT_CAMERA,
        defaultCameraDescriptorSetLayout(this->graphics.device),
        destroyDescriptorSetLayout
    );

    addResource(&this->resource, OBJECT_LAYOUT, objectLayoutData, cleanupResourceManager);
}

static void createGraphicPipelineLayouts(struct EngineCore *this) {
    struct ResourceManager *graphicPipelinesData = calloc(1, sizeof(struct ResourceManager));

    struct ResourceManager *objectData = findResource(&this->resource, OBJECT_LAYOUT);
    struct ResourceManager *textureData = findResource(&this->resource, TEXTURES);

    struct Textures *colorTexture = findResource(textureData, TEXTURE_COLOR);
    struct Textures *cubeMap = findResource(textureData, TEXTURE_CUBEMAP);

    struct descriptorSetLayout *gltfLayout = findResource(objectData, OBJECT_LAYOUT_GLTF);
    struct descriptorSetLayout *objectLayout = findResource(objectData, OBJECT_LAYOUT_OBJ);
    struct descriptorSetLayout *recLayout = findResource(objectData, OBJECT_LAYOUT_REC);
    struct descriptorSetLayout *fontLayout = findResource(objectData, OBJECT_LAYOUT_FONT);
    struct descriptorSetLayout *cameraLayout = findResource(objectData, OBJECT_LAYOUT_CAMERA);

    addResource(graphicPipelinesData, GRAPHICS_PIPELINE_LAYOUT_OBJ_CUBEMAP, createGraphicPipelineLayout((struct graphicsPipelineLayoutBuilder) {
        .descriptorSetLayout = (VkDescriptorSetLayout []){
            cameraLayout->descriptorSetLayout,
            cubeMap->descriptor.descriptorSetLayout,
            objectLayout->descriptorSetLayout,
        },
        .qDescriptorSetLayout = 3,

        .pushConstantRangeCount = 1,
        .pPushConstantRanges = (VkPushConstantRange []) {
            {
                .offset = 0,
                .size = sizeof(struct ObjPushConstants),
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT
            }
        },
        .debugName = "Cubemap"
    }, &this->graphics), destroyPipelineLayoutObj);
    addResource(graphicPipelinesData, GRAPHICS_PIPELINE_LAYOUT_GLTF, createGraphicPipelineLayout((struct graphicsPipelineLayoutBuilder) {
        .descriptorSetLayout = (VkDescriptorSetLayout []){
            cameraLayout->descriptorSetLayout,
            colorTexture->descriptor.descriptorSetLayout,
            gltfLayout->descriptorSetLayout,
        },
        .qDescriptorSetLayout = 3,

        .pushConstantRangeCount = 1,
        .pPushConstantRanges = (VkPushConstantRange []) {
            {
                .offset = 0,
                .size = sizeof(struct GltfPushConstants),
                .stageFlags = VK_SHADER_STAGE_FRAGMENT_BIT | VK_SHADER_STAGE_VERTEX_BIT
            }
        },
        .debugName = "glTF"
    }, &this->graphics), destroyPipelineLayoutObj);
    addResource(graphicPipelinesData, GRAPHICS_PIPELINE_LAYOUT_REC, createGraphicPipelineLayout((struct graphicsPipelineLayoutBuilder) {
        .descriptorSetLayout = (VkDescriptorSetLayout []){
            cameraLayout->descriptorSetLayout,
            colorTexture->descriptor.descriptorSetLayout,
            recLayout->descriptorSetLayout,
        },
        .qDescriptorSetLayout = 3,
        .debugName = "rectangle",
    }, &this->graphics), destroyPipelineLayoutObj);
    addResource(graphicPipelinesData, GRAPHICS_PIPELINE_LAYOUT_FONT, createGraphicPipelineLayout((struct graphicsPipelineLayoutBuilder) {
        .descriptorSetLayout = (VkDescriptorSetLayout []){
            cameraLayout->descriptorSetLayout,
            fontLayout->descriptorSetLayout,
        },
        .qDescriptorSetLayout = 2,

        .pushConstantRangeCount = 1,
        .pPushConstantRanges = (VkPushConstantRange []) {
            {
                .offset = 0,
                .size = sizeof(struct FontPushConstants),
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT
            }
        },
        .debugName = "font",
    }, &this->graphics), destroyPipelineLayoutObj);

    addResource(&this->resource, GRAPHIC_PIPELINE_LAYOUTS, graphicPipelinesData, cleanupResourceManager);
}

static void createGraphicPipelines(struct EngineCore *this) {
    struct ResourceManager *graphicPipelinesData = calloc(1, sizeof(struct ResourceManager));
    struct ResourceManager *renderPassCoreData = findResource(&this->resource, RENDER_PASS_CORE);
    struct ResourceManager *graphicPipelineLayout = findResource(&this->resource, GRAPHIC_PIPELINE_LAYOUTS);

    struct renderPassCore *renderPass[] = {
        findResource(renderPassCoreData, RENDER_PASS_CLEAN),
        findResource(renderPassCoreData, RENDER_PASS_STAY)
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassCore *);

    struct graphicsPipelineLayout *gltfLayout = findResource(graphicPipelineLayout, GRAPHICS_PIPELINE_LAYOUT_GLTF);
    struct graphicsPipelineLayout *fontLayout = findResource(graphicPipelineLayout, GRAPHICS_PIPELINE_LAYOUT_FONT);
    struct graphicsPipelineLayout *recLayout = findResource(graphicPipelineLayout, GRAPHICS_PIPELINE_LAYOUT_REC);
    struct graphicsPipelineLayout *cubeMapLayout = findResource(graphicPipelineLayout, GRAPHICS_PIPELINE_LAYOUT_OBJ_CUBEMAP);

    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_FLOOR, createGraphicsPipelineObj((struct GraphicsPipelineBuilder) {
        .pipelineLayout = gltfLayout->pipelineLayout,
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/vert.spv",
        .fragmentShader = "shaders/frag.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .vert = defaultGltfVert(),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_BACK_BIT,

        .debugName = "Floor"
    }, &this->graphics), destroyPipelineObj);

    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_ANIM, createGraphicsPipelineObj((struct GraphicsPipelineBuilder) {
        .pipelineLayout = gltfLayout->pipelineLayout,
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/vert.spv",
        .fragmentShader = "shaders/pFrag.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .vert = defaultGltfVert(),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_BACK_BIT,

        .debugName = "Anim"
    }, &this->graphics), destroyPipelineObj);

    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_TEXT, createGraphicsPipelineObj((struct GraphicsPipelineBuilder) {
        .pipelineLayout = fontLayout->pipelineLayout,
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/text2dV.spv",
        .fragmentShader = "shaders/textF.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .vert = defaultFontVert(),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_BACK_BIT,

        .debugName = "Text"
    }, &this->graphics), destroyPipelineObj);

    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_BUTTON, createGraphicsPipelineObj((struct GraphicsPipelineBuilder) {
        .pipelineLayout = recLayout->pipelineLayout,
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/vert2d.spv",
        .fragmentShader = "shaders/frag2d.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .vert = defaultRectVert(),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_NONE,

        .debugName = "Button"
    }, &this->graphics), destroyPipelineObj);

    addResource(graphicPipelinesData, GRAPHIC_PIPELINE_SKYBOX, createGraphicsPipelineObj((struct GraphicsPipelineBuilder) {
        .pipelineLayout = cubeMapLayout->pipelineLayout,
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/skyboxV.spv",
        .fragmentShader = "shaders/skyboxF.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .vert = defaultObjVert(),
        .operation = VK_COMPARE_OP_LESS_OR_EQUAL,
        .cullFlags = VK_CULL_MODE_BACK_BIT,

        .debugName = "Skybox",
    }, &this->graphics), destroyPipelineObj);

    addResource(&this->resource, GRAPHIC_PIPELINES, graphicPipelinesData, cleanupResourceManager);
}

static void loadSounds(struct EngineCore *this) {
    struct SoundManager *soundManager = initSoundManager();

    loadSound(soundManager, 4, "music/tile_break_fall.mp3");
    loadSound(soundManager, 3, "music/hex_gone.mp3");
    loadSound(soundManager, 2, "music/crack.mp3");
    loadSound(soundManager, 1, "music/longfall.mp3");
    loadSound(soundManager, 0, "music/EmptyMusic.mp3");

    addResource(&this->resource, SOUND_MANAGER, soundManager, cleanupSoundManager);
}

void loadResources(struct EngineCore *engine, enum state *state) {
    addTextures(engine);
    addModelData(engine);

    addRenderPassCoreData(engine);
    addObjectLayout(engine);

    createGraphicPipelineLayouts(engine);
    createGraphicPipelines(engine);

    loadSounds(engine);

    *state = LOAD_GAME;
}
