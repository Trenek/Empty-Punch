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
        findResource(entityData, "player 1"),
        findResource(entityData, "player 2")
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

    struct player playerStr[2] = {
        {
            .model = entity[1],
            .actualModel = findResource(modelData, "player"),
            .height = *(int *)findResource(sceneManagerData, "wysokosc"),
            .width = *(int *)findResource(sceneManagerData, "szerokosc"),
            .hex = entity[0],
            .movements = {
                GLFW_KEY_W,
                GLFW_KEY_S,
                GLFW_KEY_A,
                GLFW_KEY_D
            }
        },
        {
            .model = entity[2],
            .actualModel = findResource(modelData, "player"),
            .height = *(int *)findResource(sceneManagerData, "wysokosc"),
            .width = *(int *)findResource(sceneManagerData, "szerokosc"),
            .hex = entity[0],
            .movements = {
                GLFW_KEY_UP,
                GLFW_KEY_DOWN,
                GLFW_KEY_LEFT,
                GLFW_KEY_RIGHT
            }
        }
    };
    playerStr[0].x = playerStr[0].width / 2;
    playerStr[0].y = playerStr[0].height / 2;
    playerStr[1].x = playerStr[1].width / 2;
    playerStr[1].y = playerStr[1].height / 2;

    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);
    
    moveCamera(&engine->window, &renderPass[0]->camera, engine->deltaTime.deltaTime);

    while (*state == GAME && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        movePlayer(&playerStr[0], &engine->window, state, engine->deltaTime.deltaTime);
        movePlayer(&playerStr[1], &engine->window, state, engine->deltaTime.deltaTime);

        updateInstances(entity, qEntity, engine->deltaTime.deltaTime);
        drawFrame(engine, qRenderPass, renderPass, qRenderPassArr, renderPassArr);
    }
}
