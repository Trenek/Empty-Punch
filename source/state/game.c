#include <cglm/cglm.h>
#include <string.h>

#include "sound.h"
#include "engineCore.h"
#include "player.h"
#include "state.h"
#include "camera.h"
#include "commandQueue.h"

#include "entity.h"
#include "shadowInstance.h"

#include "renderPassObj.h"

#define g 9.81

void game(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *modelData = findResource(&engine->resource, MODELS);
    struct ResourceManager *entityData = findResource(&engine->resource, ENTITY);
    struct ResourceManager *screenData = findResource(&engine->resource, SCREEN_DATA);
    struct ResourceManager *renderPassCoreData = findResource(&engine->resource, RENDER_PASS_CORE);
    struct ResourceManager *sceneManagerData = findResource(&engine->resource, SCENE_INFO);
    struct ResourceManager *commandQueue = findResource(&engine->resource, COMMAND_QUEUE);

    struct Entity *entity[] = {
        findResource(entityData, ENTITY_HEX),
        findResource(entityData, ENTITY_PLAYER_1),
        findResource(entityData, ENTITY_PLAYER_2),
        findResource(entityData, ENTITY_BACKGROUND),
    };

    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);

    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, RENDER_PASS_CLEAN),
        findResource(renderPassCoreData, RENDER_PASS_STAY)
    };
    size_t qRenderPassArr = sizeof(renderPassArr) / sizeof(struct renderPassCore *);

    struct renderPassObj *renderPass[] = {
        findResource(screenData, SCREEN_SKYBOX),
        findResource(screenData, SCREEN_BASE),
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);

    struct Grip grip = createGrip((struct Grip){
        .hex = entity[0],
        .height = *(int *)findResource(sceneManagerData, SCENE_HEIGHT),
        .width = *(int *)findResource(sceneManagerData, SCENE_WIDTH),
    });

    struct player playerStr[2] = {
        {
            .model = entity[1],
            .actualModel = findResource(modelData, MODEL_PLAYER),
            .grip = &grip,
            .movements = {
                GLFW_KEY_W,
                GLFW_KEY_S,
                GLFW_KEY_A,
                GLFW_KEY_D
            },
            .x = 0,
            .y = 0
        },
        {
            .model = entity[2],
            .grip = &grip,
            .actualModel = findResource(modelData, MODEL_PLAYER),
            .movements = {
                GLFW_KEY_UP,
                GLFW_KEY_DOWN,
                GLFW_KEY_LEFT,
                GLFW_KEY_RIGHT
            },
            .x = grip.width - 1,
            .y = grip.height - 1
        }
    };

    struct shadowInstance *adam[] = {
        playerStr[0].model->instance,
        playerStr[1].model->instance
    };

    struct CommandQueue *graphics = findResource(commandQueue, COMMAND_QUEUE_GRAPHICS);
    struct CommandQueue *queue[] = {
        graphics,
    };
    size_t qQueue = sizeof(queue) / sizeof(struct CommandQueue *);

    moveCamera(&engine->window, renderPass[0]->camera, engine->deltaTime.deltaTime);
    moveCamera(&engine->window, renderPass[1]->camera, engine->deltaTime.deltaTime);

    playSound(findResource(&engine->resource, SOUND_MANAGER), 0, true, 1.0f);

    while (*state == GAME && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        movePlayer(&playerStr[0], &engine->window, state, engine->deltaTime.deltaTime, engine);
        movePlayer(&playerStr[1], &engine->window, state, engine->deltaTime.deltaTime, engine);
        movePlayer(&playerStr[0], &engine->window, state, engine->deltaTime.deltaTime, engine);
        movePlayer(&playerStr[1], &engine->window, state, engine->deltaTime.deltaTime, engine);
        if (adam[0]->pos[2] < -10) {
            *state = END;
        }
        else if (adam[1]->pos[2] < -10) {
            *state = END;
        }

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
    }

    freeGrip(grip);
}
