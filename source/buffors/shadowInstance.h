#ifndef SHADOW_INSTANCE_H
#define SHADOW_INSTANCE_H

#include <cglm/cglm.h>

#define INS(x, y) \
    .instanceSize = sizeof(struct x), \
    .instanceBufferSize = sizeof(struct y), \
    .instanceUpdater = x##Updater

struct shadowInstanceBuffer {
    uint32_t textureIndex;
    uint32_t shadow;
    mat4 modelMatrix;
};

struct shadowInstance {
    uint32_t textureIndex;
    uint32_t textureInc;
    uint32_t shadow;
    vec3 pos;
    vec3 rotation;
    vec3 fixedRotation;
    vec3 scale;
};

struct Entity;
void updateShadowInstances(struct Entity **model, size_t qModel, float deltaTime);
void shadowInstanceUpdater(void *instancePtr, void *instanceBufferPtr, uint32_t instanceCount, float deltaTime);
#endif
