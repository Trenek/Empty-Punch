#include <cglm.h>
#include <string.h>

#include <gsl/gsl_linalg.h>
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_vector.h>

#include "engineCore.h"
#include "state.h"

#include "entity.h"
#include "instanceBuffer.h"

#include "renderPassObj.h"

#define g 9.81

void game(struct EngineCore *engine, enum state *state) {
    struct ResourceManager *entityData = findResource(&engine->resource, "Entity");
    struct ResourceManager *screenData = findResource(&engine->resource, "ScreenData");

    struct Entity *entity[] = {
        findResource(entityData, "hex")
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

    size_t qRenderPass = sizeof(renderPass) / sizeof(struct renderPassObj *);

    while (*state == GAME && !shouldWindowClose(engine->window)) {
        glfwPollEvents();

        updateInstances(entity, qEntity, engine->deltaTime.deltaTime);

        moveCamera(&engine->window, &renderPass[0]->camera, engine->deltaTime.deltaTime);
        printf("\r(%f, %f, %f), (%f, %f, %f), (%f, %f)",
            renderPass[0]->camera.pos[0],
            renderPass[0]->camera.pos[1],
            renderPass[0]->camera.pos[2],
            renderPass[0]->camera.direction[0],
            renderPass[0]->camera.direction[1],
            renderPass[0]->camera.direction[2],
            renderPass[0]->camera.tilt[0],
            renderPass[0]->camera.tilt[1]
        );

        drawFrame(engine, qRenderPass, renderPass, qRenderPassArr, renderPassArr);
    }
}
