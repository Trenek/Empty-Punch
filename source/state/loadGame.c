#include <cglm/cglm.h>

#include "renderPassObj.h"
#include "engineCore.h"
#include "state.h"

#include "texture.h"
#include "shadowInstance.h"
#include "camera.h"

#include "rectangleBuilder.h"
#include "gltfBuilder.h"
#include "objBuilder.h"
#include "fontBuilder.h"
#include "entity.h"

void loadScene(struct EngineCore *this) {
    struct ResourceManager *entityData = calloc(1, sizeof(struct ResourceManager));

    addResource(entityData, SCENE_HEIGHT, malloc(sizeof(int)), free);
    addResource(entityData, SCENE_WIDTH, malloc(sizeof(int)), free);

    *(int *)findResource(entityData, SCENE_HEIGHT) = 11;
    *(int *)findResource(entityData, SCENE_WIDTH) = 11;

    addResource(&this->resource, SCENE_INFO, entityData, cleanupResourceManager);
}

void addString(
    struct ResourceManager *entityData,
    struct ResourceManager *modelData,

    struct descriptorSetLayout *objectLayout,
    struct EngineCore *this,
    size_t id,
    const char *buffer
) {
    addResource(entityData, id, createFont((struct FontBuilder) {
        .instanceCount = 1,
        .string = buffer,
        .modelData = findResource(modelData, MODEL_FONT),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(shadowInstance, shadowInstanceBuffer),
        .center = 0
    }, &this->graphics), destroyEntity);
}

static void addEntities(struct EngineCore *this) {
    struct ResourceManager *scene = findResource(&this->resource, SCENE_INFO);

    int n = *(int *)findResource(scene, SCENE_HEIGHT);
    int m = *(int *)findResource(scene, SCENE_WIDTH);
    struct ResourceManager *entityData = calloc(1, sizeof(struct ResourceManager));
    struct ResourceManager *modelData = findResource(&this->resource, MODELS);
    struct ResourceManager *objectData = findResource(&this->resource, OBJECT_LAYOUT);

    struct descriptorSetLayout *objectLayoutObj = findResource(objectData, OBJECT_LAYOUT_OBJ);
    struct descriptorSetLayout *objectLayoutGltf = findResource(objectData, OBJECT_LAYOUT_GLTF);
    struct descriptorSetLayout *objectLayoutFont = findResource(objectData, OBJECT_LAYOUT_FONT);
    struct descriptorSetLayout *objectLayoutRec = findResource(objectData, OBJECT_LAYOUT_REC);

    addResource(entityData, ENTITY_HEX, createGltf((struct GltfBuilder) {
        .instanceCount = n * m,
        .modelData = findResource(modelData, MODEL_HEX),
        .objectLayout = objectLayoutGltf->descriptorSetLayout,

        INS(shadowInstance, shadowInstanceBuffer),
    }, &this->graphics), destroyEntity);

    addResource(entityData, ENTITY_PLAYER_1, createGltf((struct GltfBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_PLAYER),
        .objectLayout = objectLayoutGltf->descriptorSetLayout,

        INS(shadowInstance, shadowInstanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, ENTITY_PLAYER_2, createGltf((struct GltfBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_PLAYER),
        .objectLayout = objectLayoutGltf->descriptorSetLayout,

        INS(shadowInstance, shadowInstanceBuffer),
    }, &this->graphics), destroyEntity);
    addString(entityData, modelData, objectLayoutFont, this, ENTITY_TEXT_MAIN_MENU, "Main Menu");
    addResource(entityData, ENTITY_FLAT_1, createRec((struct RecBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_FLAT),
        .objectLayout = objectLayoutRec->descriptorSetLayout,

        INS(shadowInstance, shadowInstanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, ENTITY_FLAT_2, createRec((struct RecBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_FLAT),
        .objectLayout = objectLayoutRec->descriptorSetLayout,

        INS(shadowInstance, shadowInstanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, ENTITY_BACKGROUND, createObj((struct ObjBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, MODEL_SKYBOX),
        .objectLayout = objectLayoutObj->descriptorSetLayout,

        INS(shadowInstance, shadowInstanceBuffer),
    }, &this->graphics), destroyEntity);
    addString(entityData, modelData, objectLayoutFont, this, ENTITY_TEXT_PLAY, "Play");
    addString(entityData, modelData, objectLayoutFont, this, ENTITY_TEXT_EXIT, "Exit");

    struct shadowInstance *hex = ((struct Entity *)findResource(entityData, ENTITY_HEX))->instance;
    struct shadowInstance *player = ((struct Entity *)findResource(entityData, ENTITY_PLAYER_1))->instance;
    struct shadowInstance *player2 = ((struct Entity *)findResource(entityData, ENTITY_PLAYER_2))->instance;

    for (int j = 0; j < m; j += 1) {
        for (int i = 0; i < (n - (j % 2)); i += 1) {
            hex[j * n + i] = (struct shadowInstance){
                .pos = { 
                    - (i - (n - 1) / 2.0) * sqrt(3) - sqrt(3) * (j % 2) / 2,
                    1.5f * (j - (m - 1) / 2.0),
                    0.0f
                },
                .rotation = { 0.0f, 0.0f, 0.0f },
                .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
                .scale = { 0.8, 1, 0.8 },
                .textureIndex = 0,
                .shadow = false,
            };
        }
    }
    player[0] = (struct shadowInstance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), glm_rad(180), 0.0f },
        .scale = { 3 * 10e-2, 3 * 10e-2, 3 * 10e-2 },
        .textureIndex = 1,
        .shadow = false,
    };

    player2[0] = (struct shadowInstance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), glm_rad(180), 0.0f },
        .scale = { 3 * 10e-2, 3 * 10e-2, 3 * 10e-2 },
        .textureIndex = 1,
        .shadow = 1,
    };

    addResource(&this->resource, ENTITY, entityData, cleanupResourceManager);
}

void loadScreens(struct EngineCore *this) {
    struct ResourceManager *screenData = calloc(1, sizeof(struct ResourceManager));

    struct ResourceManager *graphicsPipelineData = findResource(&this->resource, GRAPHIC_PIPELINES);
    struct ResourceManager *entityData = findResource(&this->resource, ENTITY);

    struct graphicsPipeline *pipe[] = {
        findResource(graphicsPipelineData, GRAPHIC_PIPELINE_FLOOR),
        findResource(graphicsPipelineData, GRAPHIC_PIPELINE_TEXT),
        findResource(graphicsPipelineData, GRAPHIC_PIPELINE_ANIM),
        findResource(graphicsPipelineData, GRAPHIC_PIPELINE_BUTTON),
        findResource(graphicsPipelineData, GRAPHIC_PIPELINE_SKYBOX),
    };

    struct ResourceManager *renderPassCoreData = findResource(&this->resource, RENDER_PASS_CORE);
    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, RENDER_PASS_CLEAN),
        findResource(renderPassCoreData, RENDER_PASS_STAY)
    };

    struct descriptorSetLayout *cameraLayout = findResource(findResource(&this->resource, OBJECT_LAYOUT), OBJECT_LAYOUT_CAMERA);

    struct Entity *entity[] = {
        findResource(entityData, ENTITY_TEXT_MAIN_MENU),
        findResource(entityData, ENTITY_FLAT_1),
        findResource(entityData, ENTITY_BACKGROUND),
        findResource(entityData, ENTITY_TEXT_PLAY),
        findResource(entityData, ENTITY_TEXT_EXIT),
        findResource(entityData, ENTITY_FLAT_2),
    };

    struct Textures *colorTexture = findResource(findResource(&this->resource, TEXTURES), TEXTURE_COLOR);
    struct Textures *cubemapTexture = findResource(findResource(&this->resource, TEXTURES), TEXTURE_CUBEMAP);

    addResource(screenData, SCREEN_BASE, createRenderPassObj((struct renderPassBuilder){
        .renderPass = renderPassArr[1],
        .color = { 0.0, 0.0, 0.0, 1.0 },
        .coordinates = { 0.0, 0.0, 1.0, 1.0 },
        .data = (struct pipelineConnection[]) {
            {
                .texture = &colorTexture->descriptor,
                .pipe = pipe[0],
                .entity = (struct Entity* []) {
                    findResource(entityData, ENTITY_HEX),
                },
                .qEntity = 1
            },
            {
                .texture = &colorTexture->descriptor,
                .pipe = pipe[2],
                .entity = (struct Entity* []) {
                    findResource(entityData, ENTITY_PLAYER_1),
                    findResource(entityData, ENTITY_PLAYER_2),
                },
                .qEntity = 2
            },
        },
        .qData = 2,
        .updateCameraBuffer = myUpdateFirstPersonCameraBuffer,
        .cameraSize = sizeof(struct camera),
        .cameraBufferSize = sizeof(struct CameraBuffer),
        .camera = &(struct camera){
            .pos = { 0.0, 10.0, 20.0 },
            .direction = { 0.0, -10.0, -20.0 }
        },
        .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
    }, &this->graphics), destroyRenderPassObj);
    addResource(screenData, SCREEN_SKYBOX, createRenderPassObj((struct renderPassBuilder){
        .renderPass = renderPassArr[0],
        .coordinates = { 0.0, 0.0, 1.0, 1.0 },
        .data = (struct pipelineConnection[]) {
            {
                .texture = &cubemapTexture->descriptor,
                .pipe = pipe[4],
                .entity = &entity[2],
                .qEntity = 1
            }
        },
        .qData = 1,
        .cameraSize = sizeof(struct camera),
        .cameraBufferSize = sizeof(struct CameraBuffer),
        .camera = &(struct camera){},
        .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
        .updateCameraBuffer = myUpdateFirstPersonCameraBuffer,
    }, &this->graphics), destroyRenderPassObj);
    addResource(screenData, SCREEN_INTERFACE, createRenderPassObj((struct renderPassBuilder){
        .renderPass = renderPassArr[1],
        .coordinates = { 0.0, 0.0, 1.0, 1.0 },
        .data = (struct pipelineConnection[]) {
            {
                .pipe = pipe[1],
                .entity = (struct Entity* []) {
                    entity[0],
                    entity[3],
                    entity[4],
                },
                .qEntity = 3
            },
            {
                .texture = &colorTexture->descriptor,
                .pipe = pipe[3],
                .entity = (struct Entity* []) {
                    entity[1],
                    entity[5],
                },
                .qEntity = 2
            },
        },
        .qData = 2,
        .cameraSize = sizeof(struct camera),
        .camera = &(struct camera){},
        .cameraBufferSize = sizeof(struct CameraBuffer),
        .updateCameraBuffer = myUpdateFirstPersonCameraBuffer,
        .cameraDescriptorSetLayout = cameraLayout->descriptorSetLayout,
    }, &this->graphics), destroyRenderPassObj),

    addResource(&this->resource, SCREEN_DATA, screenData, cleanupResourceManager);
}

void loadGame(struct EngineCore *engine, enum state *state) {
    loadScene(engine);
    addEntities(engine);

    loadScreens(engine);
    *state = MAIN_MENU;
}
