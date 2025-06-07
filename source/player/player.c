#include "state.h"
#include "player.h"
#include "entity.h"

#include "instanceBuffer.h"
#include "windowManager.h"
#include "actualModel.h"

bool checkForColisionToAdd(struct player *p, const char *name, vec3 toAdd);
bool checkForColision(struct player *p, const char *name);

void animate(struct Entity *model, struct actualModel *actualModel, size_t animID, float deltaTime);

void posePlayer(struct player *p, float deltaTime) {
    p->time += deltaTime;

    animate(p->model, p->actualModel, p->state, p->time);
}
