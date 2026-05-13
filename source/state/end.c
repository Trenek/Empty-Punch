#include <stdlib.h>

#include "engineCore.h"
#include "state.h"

#include "renderPassObj.h"
#include "commandQueue.h"

float genRand(float x) {
    float result = (double)rand() / (double)RAND_MAX;

    return (result + x > 1.0) ? 1.0 - x - 0.1 : result;
}

void end(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *renderPassCoreData = findResource(&engine->resource, RENDER_PASS_CORE);
    struct ResourceManager *commandQueue = findResource(&engine->resource, COMMAND_QUEUE);
    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, RENDER_PASS_CLEAN),
        findResource(renderPassCoreData, RENDER_PASS_STAY)
    };
    size_t qRenderPassArr = sizeof(renderPassArr) / sizeof(struct renderPassCore *);

    struct renderPassObj *renderPass[] = {
        createRenderPassObj((struct renderPassBuilder){
            .renderPass = renderPassArr[1],
            .color = { 0.0, 0.0, 0.0, 1.0 },
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .drawRenderPass = drawRenderPass
        }, &engine->graphics),
        createRenderPassObj((struct renderPassBuilder){
            .renderPass = renderPassArr[0],
            .color = { 0.0, 0.0, 0.0, 1.0 },
            .coordinates = { 0.0, 0.0, 0.1, 0.1 },
            .drawRenderPass = drawRenderPass
        }, &engine->graphics),
        createRenderPassObj((struct renderPassBuilder){
            .renderPass = renderPassArr[1],
            .color = { 0.0, 0.0, 0.0, 1.0 },
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .drawRenderPass = drawRenderPass
        }, &engine->graphics)
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);

    float time = 5;

    struct CommandQueue *graphics = findResource(commandQueue, COMMAND_QUEUE_GRAPHICS);
    struct CommandQueue *queue[] = {
        graphics,
    };
    size_t qQueue = sizeof(queue) / sizeof(struct CommandQueue *);
    while (time > 0) {
        renderPass[1]->coordinates[0] = genRand(0.1);
        renderPass[1]->coordinates[1] = genRand(0.1);
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

        time -= engine->deltaTime.deltaTime;
    }

    *state = EXIT;

    vkDeviceWaitIdle(engine->graphics.device);
    destroyRenderPassObjArr(qRenderPass, renderPass);
}
