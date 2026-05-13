#include <cglm/cglm.h>
#include <string.h>

#include "defaultCamera.h"

#include "engineCore.h"
#include "state.h"
#include "commandQueue.h"

#include "entity.h"
#include "shadowInstance.h"

#include "renderPassObj.h"

#include "player.h"

#include "button.h"

void menu(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *entityData = findResource(&engine->resource, ENTITY);
    struct ResourceManager *modelData = findResource(&engine->resource, MODELS);
    struct ResourceManager *renderPassCoreData = findResource(&engine->resource, RENDER_PASS_CORE);
    struct ResourceManager *renderPassData = findResource(&engine->resource, SCREEN_DATA);
    struct ResourceManager *commandQueue = findResource(&engine->resource, COMMAND_QUEUE);

    struct Entity *entity[] = {
        findResource(entityData, ENTITY_TEXT_MAIN_MENU),
        findResource(entityData, ENTITY_FLAT_1),
        findResource(entityData, ENTITY_BACKGROUND),
        findResource(entityData, ENTITY_TEXT_PLAY),
        findResource(entityData, ENTITY_TEXT_EXIT),
        findResource(entityData, ENTITY_FLAT_2),
    };
    [[maybe_unused]]
    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);

    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, RENDER_PASS_CLEAN),
        findResource(renderPassCoreData, RENDER_PASS_STAY)
    };
    size_t qRenderPassArr = sizeof(renderPassArr) / sizeof(struct renderPassCore *);

    struct renderPassObj *renderPass[] = {
        findResource(renderPassData, SCREEN_SKYBOX),
        findResource(renderPassData, SCREEN_INTERFACE)
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);

    struct Button button = {
        .joystick = GLFW_JOYSTICK_1,
        .isClicked = false,
        .qButton = 2,
        .entity = (struct Entity *[]){
            entity[1],
            entity[5],
        },
        .model = findResource(modelData, MODEL_FLAT),
        .camera = renderPass[1]->cameraBuffer.buffersMapped[0],
        .newState = (int []) {
            GAME,
            EXIT
        },
        .chosen = 0,
    };

    struct shadowInstance *text = entity[0]->instance;
    struct shadowInstance *flat[] = {
        entity[1]->instance,
        entity[5]->instance,
    };
    struct shadowInstance *background = entity[2]->instance;
    struct shadowInstance *buttonText[] = {
        entity[3]->instance,
        entity[4]->instance
    };

    struct CommandQueue *graphics = findResource(commandQueue, COMMAND_QUEUE_GRAPHICS);
    struct CommandQueue *queue[] = {
        graphics,
    };
    size_t qQueue = sizeof(queue) / sizeof(struct CommandQueue *);

    text[0] = (struct shadowInstance){
        .pos = { 0.0f, 0.3f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 6 * 10e-7, 6 * 10e-7, 6 * 10e-7 },
        .textureIndex = 0,
        .shadow = false
    };

    buttonText[0][0] = (struct shadowInstance){
        .pos = { 0.0f, -0.014f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 4 * 10e-7, 4 * 10e-7, 4 * 10e-7 },
        .textureIndex = 0,
        .shadow = false
    };

    buttonText[1][0] = (struct shadowInstance){
        .pos = { 0.0f, -0.11f -0.014f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 4 * 10e-7, 4 * 10e-7, 4 * 10e-7 },
        .textureIndex = 0,
        .shadow = false
    };

    flat[0][0] = (struct shadowInstance){
        .pos = { 0.0f, 0.0f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 0.1f, 0.05f, 0.1f },
        .textureIndex = 3,
        .shadow = false
    };

    flat[1][0] = (struct shadowInstance){
        .pos = { 0.0f, -0.11f, 0.0f },
        .rotation = { 0.0f, 0.0f, 0.0f },
        .fixedRotation = { 0.0f, 0.0f, 0.0f },
        .scale = { 0.1f, 0.05f, 0.1f },
        .textureIndex = 3,
        .shadow = false
    };

    background[0] = (struct shadowInstance){
        .pos = { 0.0f, 0.0f, 0.0f }, 
        .rotation = { 0.0f, glm_rad(0.3), 0.0f },
        .fixedRotation = { glm_rad(90), 0.0f, 0.0f },
        .scale = { 1.0f, 1.0f, 1.0f },
        .textureIndex = 0,
        .shadow = false
    };

    *(struct FirstPerson *)renderPass[0]->camera = (struct FirstPerson) {
        .pos = { 0.0, 0.0, 0.0 },
        .direction = { 0.0, 1.0, 0.0 }
    };
    *(struct FirstPerson *)renderPass[1]->camera = (struct FirstPerson) {
        .pos = { 0.0, 0.0, 0.0 },
        .direction = { 0.0, 1.0, 0.0 }
    };

    while (MAIN_MENU == *state && !shouldWindowClose(engine->window)) {
        updateShadowInstances(entity, qEntity, engine->deltaTime.deltaTime);

        engineUpdate(engine, qRenderPass, renderPass);
        
        aquireNextImage(engine, graphics->inFlightFence, graphics->semaphore);

        queueDraw(graphics, engine, qRenderPass, renderPass, 1, 
            (VkSemaphore []) {
                graphics->semaphore[engine->currentFrame],
            },
            (VkPipelineStageFlags []) {
                VK_PIPELINE_STAGE_COLOR_ATTACHMENT_OUTPUT_BIT,
            }
        );

        presentFrame(engine, qRenderPassArr, renderPassArr, qQueue, queue);
        shadowButton(engine->graphics, engine->window, &button);
        if (button.isClicked) {
            *state = button.newState[button.chosen];
        }
    }
}
