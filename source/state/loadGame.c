#include <cglm.h>

#include "renderPassObj.h"
#include "engineCore.h"
#include "state.h"

#include "asset.h"
#include "instanceBuffer.h"

#include "modelBuilder.h"
#include "stringBuilder.h"
#include "entity.h"

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
    struct ResourceManager *entityData = calloc(1, sizeof(struct ResourceManager));
    struct ResourceManager *modelData = findResource(&this->resource, "modelData");

    struct descriptorSetLayout *objectLayout = findResource(findResource(&this->resource, "objectLayout"), "object");

    addResource(entityData, "hex", createModel((struct ModelBuilder) {
        .instanceCount = 1,
        .modelData = findResource(modelData, "hex"),
        .objectLayout = objectLayout->descriptorSetLayout,

        INS(instance, instanceBuffer),
    }, &this->graphics), destroyEntity);

    addResource(&this->resource, "Entity", entityData, cleanupResources);
}

void loadScreens(struct EngineCore *this) {
    struct ResourceManager *screenData = calloc(1, sizeof(struct ResourceManager));

    struct ResourceManager *graphicsPipelineData = findResource(&this->resource, "graphicPipelines");
    struct ResourceManager *entityData = findResource(&this->resource, "Entity");

    struct graphicsPipeline *pipe[] = {
        findResource(graphicsPipelineData, "Floor"),
        findResource(graphicsPipelineData, "Text"),
    };

    struct ResourceManager *renderPassCoreData = findResource(&this->resource, "RenderPassCoreData");
    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, "Clean"),
        findResource(renderPassCoreData, "Stay")
    };

    addResource(screenData, "Base Screen", createRenderPassObj((struct renderPassBuilder){
        .renderPass = renderPassArr[0],
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
        },
        .qData = 1,
        .camera = {
            .pos = { 0.0, 2.0, 4.0 },
            .direction = { 0.0, -0.5, -1.0 }
        },
        .updateCameraBuffer = updateFirstPersonCameraBuffer,
    }, &this->graphics), destroyRenderPassObj);

    struct instance *hex = ((struct Entity *)findResource(entityData, "hex"))->instance;
    *hex = (struct instance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 1, 1, 1 },
        .textureIndex = 0,
        .shadow = false,
        .color = { 1.0, 1.0, 1.0, 1.0 }
    };

    addResource(&this->resource, "ScreenData", screenData, cleanupResources);
}

void loadGame(struct EngineCore *engine, enum state *state) {
    addEntities(engine);

    loadScreens(engine);

    *state = GAME;
}
