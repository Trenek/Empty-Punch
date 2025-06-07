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

// lewa strona - *, modyfikatory, typy proste, np. int, char
// prawa strona - [], ()

char (*(*(*returnArray(int n))[])())[50] {
    char (*(*(*array)[])())[50] = malloc(sizeof(char (*(*(*)[])())[50]) * n);

    return array;
}

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
        .actualModel = findResource(modelData, "player")
    };

    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);
    
    struct instance *player = entity[1]->instance;

    while (*state == GAME && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        posePlayer(&playerStr, engine->deltaTime.deltaTime);
        updateInstances(entity, qEntity, engine->deltaTime.deltaTime);

        moveCamera(&engine->window, &renderPass[0]->camera, engine->deltaTime.deltaTime);
        
        if ((KEY_PRESS | KEY_REPEAT) == getKeyState(&engine->window, GLFW_KEY_T)) player[0].pos[1] -= 0.01;
        if ((KEY_PRESS | KEY_REPEAT) == getKeyState(&engine->window, GLFW_KEY_G)) player[0].pos[1] += 0.01;
        if ((KEY_PRESS | KEY_REPEAT) == getKeyState(&engine->window, GLFW_KEY_F)) player[0].pos[0] += 0.01;
        if ((KEY_PRESS | KEY_REPEAT) == getKeyState(&engine->window, GLFW_KEY_H)) player[0].pos[0] -= 0.01;

        drawFrame(engine, qRenderPass, renderPass, qRenderPassArr, renderPassArr);
    }
}
