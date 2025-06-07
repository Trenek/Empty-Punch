#include <cglm.h>
#include <string.h>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#include "engineCore.h"
#include "player.h"
#include "state.h"

#include "entity.h"
#include "instanceBuffer.h"

#include "renderPassObj.h"

#define g 9.81

void game(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *modelData = findResource(&engine->resource, "modelData");
    struct ResourceManager *entityData = findResource(&engine->resource, "Entity");
    struct ResourceManager *screenData = findResource(&engine->resource, "ScreenData");

    struct Entity *entity[] = {
        findResource(entityData, "hex"),
        findResource(entityData, "player")
    };

    size_t qEntity = sizeof(entity) / sizeof(struct Entity *);

    struct ResourceManager *renderPassCoreData = findResource(&engine->resource, "RenderPassCoreData");
    struct ResourceManager *sceneManagerData = findResource(&engine->resource, "scena");
    struct renderPassCore *renderPassArr[] = { 
        findResource(renderPassCoreData, "Clean"),
        findResource(renderPassCoreData, "Stay")
    };
    size_t qRenderPassArr = sizeof(renderPassArr) / sizeof(struct renderPassCore *);

    struct renderPassObj *renderPass[] = {
        findResource(screenData, "Base Screen")
    };

    struct player playerStr = {
        .model = entity[1],
        .actualModel = findResource(modelData, "player"),
        .height = *(int *)findResource(sceneManagerData, "wysokosc"),
        .width = *(int *)findResource(sceneManagerData, "szerokosc"),
        .hex = entity[0]
    };
    playerStr.x = playerStr.width / 2;
    playerStr.y = playerStr.height / 2;

    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);
    
    while (*state == GAME && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        movePlayer(&playerStr, &engine->window, engine->deltaTime.deltaTime);
        updateInstances(entity, qEntity, engine->deltaTime.deltaTime);

        moveCamera(&engine->window, &renderPass[0]->camera, engine->deltaTime.deltaTime);
        drawFrame(engine, qRenderPass, renderPass, qRenderPassArr, renderPassArr);
    }
}
