#include <cglm.h>

#include "engineCore.h"
#include "state.h"

#include "asset.h"
#include "entity.h"
#include "modelBuilder.h"
#include "stringBuilder.h"

#include "renderPassCore.h"

#include "graphicsPipelineObj.h"

#include "Vertex.h"

static void addTextures(struct EngineCore *this) {
    struct ResourceManager *textureManager = calloc(1, sizeof(struct ResourceManager));

    addResource(textureManager, "Color", loadTextures(&this->graphics, 3, (const char *[]){
        "textures/hex_small.png",
        "textures/player1.png",
        "textures/player.png",
        "textures/hex_cracked.png",
    }), unloadTextures);
    addResource(textureManager, "cubeMap", loadCubeMaps(&this->graphics, (const char *[]) {
        "textures/CubeMap_purple/xpos.png",
        "textures/CubeMap_purple/xneg.png",
        "textures/CubeMap_purple/ypos.png",
        "textures/CubeMap_purple/yneg.png",
        "textures/CubeMap_purple/zpos.png",
        "textures/CubeMap_purple/zneg.png",
    }), unloadTextures);

    addResource(&this->resource, "textures", textureManager, cleanupResources);
}

static void addModelData(struct EngineCore *this) {
    struct ResourceManager *modelData = calloc(1, sizeof(struct ResourceManager));

    addResource(modelData, "player", loadModel("models/player.glb", &this->graphics), destroyActualModel);
    addResource(modelData, "hex", loadModel("models/hex.glb", &this->graphics), destroyActualModel);
    addResource(modelData, "font", loadModel("fonts/c.ttf", &this->graphics), destroyActualModel);
    addResource(modelData, "flat", loadModel("models/my_model2d.obj", &this->graphics), destroyActualModel);
    addResource(modelData, "skyBox", loadModel("models/my_skybox.obj", &this->graphics), destroyActualModel);

    addResource(&this->resource, "modelData", modelData, cleanupResources);
}

static void addRenderPassCoreData(struct EngineCore *this) {
    struct ResourceManager *renderPassCoreData = calloc(1, sizeof(struct ResourceManager));

    addResource(renderPassCoreData, "Clean", createRenderPassCore((struct renderPassCoreBuilder) {
        .loadOp = VK_ATTACHMENT_LOAD_OP_CLEAR,
        .initLayout = VK_IMAGE_LAYOUT_UNDEFINED
    }, &this->graphics), freeRenderPassCore);
    addResource(renderPassCoreData, "Stay", createRenderPassCore((struct renderPassCoreBuilder) {
        .loadOp = VK_ATTACHMENT_LOAD_OP_LOAD,
        .initLayout = VK_IMAGE_LAYOUT_COLOR_ATTACHMENT_OPTIMAL
    }, &this->graphics), freeRenderPassCore);

    addResource(&this->resource, "RenderPassCoreData", renderPassCoreData, cleanupResources);
}

static void addObjectLayout(struct EngineCore *this) {
    struct ResourceManager *objectLayoutData = calloc(1, sizeof(struct ResourceManager));

    addResource(objectLayoutData, "object", createDescriptorSetLayout(
        createObjectDescriptorSetLayout(this->graphics.device, 2, (VkDescriptorSetLayoutBinding []) {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = NULL
            },
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = NULL
            }
        }), this->graphics.device), 
        destroyDescriptorSetLayout
    );
    addResource(objectLayoutData, "anim", createDescriptorSetLayout(
        createObjectDescriptorSetLayout(this->graphics.device, 3, (VkDescriptorSetLayoutBinding []) {
            {
                .binding = 0,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = NULL
            },
            {
                .binding = 1,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = NULL
            },
            {
                .binding = 2,
                .descriptorType = VK_DESCRIPTOR_TYPE_STORAGE_BUFFER,
                .descriptorCount = 1,
                .stageFlags = VK_SHADER_STAGE_VERTEX_BIT,
                .pImmutableSamplers = NULL
            },
        }), this->graphics.device), 
        destroyDescriptorSetLayout
    );
    addResource(objectLayoutData, "camera", 
        createDescriptorSetLayout(createCameraDescriptorSetLayout(this->graphics.device), this->graphics.device), 
        destroyDescriptorSetLayout
    );

    addResource(&this->resource, "objectLayout", objectLayoutData, cleanupResources);
}

static void createGraphicPipelines(struct EngineCore *this) {
    struct ResourceManager *graphicPipelinesData = calloc(1, sizeof(struct ResourceManager));
    struct ResourceManager *renderPassCoreData = findResource(&this->resource, "RenderPassCoreData");
    struct ResourceManager *objectData = findResource(&this->resource, "objectLayout");
    struct ResourceManager *textureData = findResource(&this->resource, "textures");

    struct Textures *colorTexture = findResource(textureData, "Color");
    struct Textures *cubeMap = findResource(textureData, "cubeMap");

    struct descriptorSetLayout *objectLayout = findResource(objectData, "object");
    struct descriptorSetLayout *animLayout = findResource(objectData, "anim");
    struct descriptorSetLayout *cameraLayout = findResource(objectData, "camera");

    struct renderPassCore *renderPass[] = {
        findResource(renderPassCoreData, "Clean"),
        findResource(renderPassCoreData, "Stay")
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassCore *);

    addResource(graphicPipelinesData, "Floor", createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/vert.spv",
        .fragmentShader = "shaders/frag2.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .texture = &colorTexture->descriptor,
        .objectLayout = objectLayout->descriptorSetLayout,

        Vert(AnimVertex),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_BACK_BIT,

        .cameraLayout = cameraLayout->descriptorSetLayout
    }, &this->graphics), destroyObjGraphicsPipeline);
    addResource(graphicPipelinesData, "Animated Model", createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/animation.spv",
        .fragmentShader = "shaders/frag2.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .texture = &colorTexture->descriptor,
        .objectLayout = animLayout->descriptorSetLayout,

        Vert(AnimVertex),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_BACK_BIT,

        .cameraLayout = cameraLayout->descriptorSetLayout
    }, &this->graphics), destroyObjGraphicsPipeline);
    addResource(graphicPipelinesData, "Text", createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/text2dV.spv",
        .fragmentShader = "shaders/textF.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .texture = &colorTexture->descriptor,
        .objectLayout = objectLayout->descriptorSetLayout,

        Vert(FontVertex),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_BACK_BIT,

        .cameraLayout = cameraLayout->descriptorSetLayout
    }, &this->graphics), destroyObjGraphicsPipeline);
    addResource(graphicPipelinesData, "Flat", createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/vert2d.spv",
        .fragmentShader = "shaders/frag2d.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .texture = &colorTexture->descriptor,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .objectLayout = objectLayout->descriptorSetLayout,

        Vert(AnimVertex),
        .operation = VK_COMPARE_OP_LESS,
        .cullFlags = VK_CULL_MODE_NONE,

        .cameraLayout = cameraLayout->descriptorSetLayout
    }, &this->graphics), destroyObjGraphicsPipeline);
    addResource(graphicPipelinesData, "Skybox", createObjGraphicsPipeline((struct graphicsPipelineBuilder) {
        .qRenderPassCore = qRenderPass,
        .renderPassCore = renderPass,
        .vertexShader = "shaders/skyboxV.spv",
        .fragmentShader = "shaders/skyboxF.spv",
        .minDepth = 0.0f,
        .maxDepth = 1.0f,
        .texture = &cubeMap->descriptor,
        .topology = VK_PRIMITIVE_TOPOLOGY_TRIANGLE_LIST,

        .objectLayout = objectLayout->descriptorSetLayout,

        Vert(AnimVertex),
        .operation = VK_COMPARE_OP_LESS_OR_EQUAL,
        .cullFlags = VK_CULL_MODE_BACK_BIT,

        .cameraLayout = cameraLayout->descriptorSetLayout
    }, &this->graphics), destroyObjGraphicsPipeline);

    addResource(&this->resource, "graphicPipelines", graphicPipelinesData, cleanupResources);
}

static void loadSounds(struct EngineCore *) {}

void loadResources(struct EngineCore *engine, enum state *state) {
    addTextures(engine);
    addModelData(engine);

    addRenderPassCoreData(engine);
    addObjectLayout(engine);

    createGraphicPipelines(engine);

    loadSounds(engine);

    *state = LOAD_GAME;
}
