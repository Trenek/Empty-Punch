#include <cglm.h>

#include "renderPassObj.h"
#include "engineCore.h"
#include "state.h"

#include "asset.h"
#include "instanceBuffer.h"

#include "modelBuilder.h"
#include "stringBuilder.h"
#include "entity.h"

void loadScene(struct EngineCore *this) {
    struct ResourceManager *entityData = calloc(1, sizeof(struct ResourceManager));

    addResource(entityData, "wysokosc", malloc(sizeof(int)), free);
    addResource(entityData, "szerokosc", malloc(sizeof(int)), free);

    *(int *)findResource(entityData, "wysokosc") = 11;
    *(int *)findResource(entityData, "szerokosc") = 11;

    addResource(&this->resource, "scena", entityData, cleanupResourceManager);
}

void addString(
    struct ResourceManager *entityData,
    struct ResourceManager *modelData,

    struct descriptorSetLayout *objectLayout,
    struct EngineCore *this,
    const char *name,
    const char *buffer
) {
    addResource(entityData, name, createString((struct StringBuilder) {
        .instanceCount = 1,
        .string = buffer,
        .modelData = findResource(modelData, "font"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
        .center = 0
    }, &this->graphics), destroyEntity);
}

static void addEntities(struct EngineCore *this) {
    struct ResourceManager *scene = findResource(&this->resource, "scena");

    int n = *(int *)findResource(scene, "wysokosc");
    int m = *(int *)findResource(scene, "szerokosc");
    struct ResourceManager *entityData = calloc(1, sizeof(struct ResourceManager));
    struct ResourceManager *modelData = findResource(&this->resource, "modelData");
    struct ResourceManager *objectData = findResource(&this->resource, "objectLayout");

    struct descriptorSetLayout *objectLayout = findResource(objectData, "object");
    struct descriptorSetLayout *animLayout = findResource(objectData, "anim");

    addResource(entityData, "hex", createModel((struct ModelBuilder) {
        .instanceCount = n * m,
        .modelData = findResource(modelData, "hex"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);

    addResource(entityData, "player 1", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "player"),
        .objectLayout = animLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, "player 2", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "player"),
        .objectLayout = animLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addString(entityData, modelData, objectLayout, this, "Main Menu", "Main Menu");
    addResource(entityData, "Flat 1", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "flat"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Flat 2", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "flat"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addResource(entityData, "Background", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "skyBox"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);
    addString(entityData, modelData, objectLayout, this, "Play", "Play");
    addString(entityData, modelData, objectLayout, this, "Exit", "Exit");

    struct instance *hex = ((struct Entity *)findResource(entityData, "hex"))->instance;
    struct instance *player = ((struct Entity *)findResource(entityData, "player 1"))->instance;
    struct instance *player2 = ((struct Entity *)findResource(entityData, "player 2"))->instance;

    for (int j = 0; j < m; j += 1) {
        for (int i = 0; i < (n - (j % 2)); i += 1) {
            hex[j * n + i] = (struct instance){
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
                .color = { 
                    j % 3 == 0 ? 1.0 / (j + 1) : 1.0 / (i + 1), 
                    (j + 1) % 3 == 0 ? 1.0 / (j + 1) : 1.0 / (i + 1), 
                    (j + 2) % 3 == 0 ? 1.0 / (j + 1) : 1.0 / (i + 1), 
                    1.0
                }
            };
        }
    }
    player[0] = player2[0] = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), glm_rad(180), 0.0f },
        .scale = { 3 * 10e-2, 3 * 10e-2, 3 * 10e-2 },
        .textureIndex = 1,
        .shadow = false,
    };

    addResource(&this->resource, "Entity", entityData, cleanupResourceManager);
}

void loadScreens(struct EngineCore *this) {
    struct ResourceManager *screenData = calloc(1, sizeof(struct ResourceManager));

    struct ResourceManager *graphicsPipelineData = findResource(&this->resource, "graphicPipelines");
    struct ResourceManager *entityData = findResource(&this->resource, "Entity");

    struct graphicsPipeline *pipe[] = {
        findResource(graphicsPipelineData, "Floor"),
        findResource(graphicsPipelineData, "Text"),
        findResource(graphicsPipelineData, "Animated Model"),
        findResource(graphicsPipelineData, "Flat"),
        findResource(graphicsPipelineData, "Skybox"),
    };

    struct ResourceManager *renderPassCoreData = findResource(&this->resource, "RenderPassCoreData");
    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, "Clean"),
        findResource(renderPassCoreData, "Stay")
    };

    struct Entity *entity[] = {
        findResource(entityData, "Main Menu"),
        findResource(entityData, "Flat 1"),
        findResource(entityData, "Background"),
        findResource(entityData, "Play"),
        findResource(entityData, "Exit"),
        findResource(entityData, "Flat 2"),
    };

    addResource(screenData, "Base Screen", createRenderPassObj((struct renderPassBuilder){
        .renderPass = renderPassArr[1],
        .color = { 0.0, 0.0, 0.0, 1.0 },
        .coordinates = { 0.0, 0.0, 1.0, 1.0 },
        .data = (struct pipelineConnection[]) {
            {
                .pipe = pipe[0],
                .entity = (struct Entity* []) {
                    findResource(entityData, "hex"),
                },
                .qEntity = 1
            },
            {
                .pipe = pipe[2],
                .entity = (struct Entity* []) {
                    findResource(entityData, "player 1"),
                    findResource(entityData, "player 2"),
                },
                .qEntity = 2
            },
        },
        .qData = 2,
        .camera = {
            .pos = { 0.0, 10.0, 20.0 },
            .direction = { 0.0, -10.0, -20.0 }
        },
        .updateCameraBuffer = updateFirstPersonCameraBuffer,
    }, &this->graphics), destroyRenderPassObj);
    addResource(screenData, "Skybox", createRenderPassObj((struct renderPassBuilder){
        .renderPass = renderPassArr[0],
        .coordinates = { 0.0, 0.0, 1.0, 1.0 },
        .data = (struct pipelineConnection[]) {
            {
                .pipe = pipe[4],
                .entity = &entity[2],
                .qEntity = 1
            }
        },
        .qData = 1,
        .updateCameraBuffer = updateFirstPersonCameraBuffer
    }, &this->graphics), destroyRenderPassObj);
    addResource(screenData, "Interface", createRenderPassObj((struct renderPassBuilder){
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
                .pipe = pipe[3],
                .entity = (struct Entity* []) {
                    entity[1],
                    entity[5],
                },
                .qEntity = 2
            },
        },
        .qData = 2,
        .updateCameraBuffer = updateFirstPersonCameraBuffer
    }, &this->graphics), destroyRenderPassObj),

    addResource(&this->resource, "ScreenData", screenData, cleanupResourceManager);
}

void loadGame(struct EngineCore *engine, enum state *state) {
    loadScene(engine);
    addEntities(engine);

    loadScreens(engine);

    *state = MAIN_MENU;
}
