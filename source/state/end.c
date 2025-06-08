#include "engineCore.h"
#include "state.h"

#include "renderPassObj.h"

float genRand(float x) {
    float result = (double)rand() / (double)RAND_MAX;

    return (result + x > 1.0) ? 1.0 - x - 0.1 : result;
}

void end(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *renderPassCoreData = findResource(&engine->resource, "RenderPassCoreData");
    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, "Clean"),
        findResource(renderPassCoreData, "Stay")
    };
    size_t qRenderPassArr = sizeof(renderPassArr) / sizeof(struct renderPassCore *);

    struct renderPassObj *renderPass[] = {
        createRenderPassObj((struct renderPassBuilder){
            .renderPass = renderPassArr[1],
            .color = { 0.0, 0.0, 0.0, 1.0 },
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .updateCameraBuffer = updateFirstPersonCameraBuffer,
        }, &engine->graphics),
        createRenderPassObj((struct renderPassBuilder){
            .renderPass = renderPassArr[0],
            .color = { 0.0, 0.0, 0.0, 1.0 },
            .coordinates = { 0.0, 0.0, 0.1, 0.1 },
            .updateCameraBuffer = updateFirstPersonCameraBuffer,
        }, &engine->graphics),
        createRenderPassObj((struct renderPassBuilder){
            .renderPass = renderPassArr[1],
            .color = { 0.0, 0.0, 0.0, 1.0 },
            .coordinates = { 0.0, 0.0, 1.0, 1.0 },
            .updateCameraBuffer = updateFirstPersonCameraBuffer,
        }, &engine->graphics)
    };
    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);

    float time = 5;
    while (time > 0) {
        renderPass[1]->coordinates[0] = genRand(0.1);
        renderPass[1]->coordinates[1] = genRand(0.1);
        drawFrame(engine, qRenderPass, renderPass, qRenderPassArr, renderPassArr);

        time -= engine->deltaTime.deltaTime;
    }

    *state = EXIT;

    vkDeviceWaitIdle(engine->graphics.device);
    destroyRenderPassObjArr(qRenderPass, renderPass);
}
