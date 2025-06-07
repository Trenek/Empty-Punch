#include <string.h>

#include "player.h"
#include "entity.h"

#include "instanceBuffer.h"
#include "windowManager.h"
#include "actualModel.h"

bool checkForColisionToAdd(struct player *p, const char *name, vec3 toAdd);
bool checkForColision(struct player *p, const char *name);

void animate(struct Entity *model, struct actualModel *actualModel, size_t animID, float deltaTime);

void movePlayer(struct player *p, struct WindowManager *window, float deltaTime) {
    struct instance *player = p->model->instance;
    struct instance *hex = p->hex->instance;

    p->time += deltaTime;

    int index = p->x + p->width * p->y;

    memcpy(player->pos, hex[index].pos, sizeof(float [3]));
        
    if ((KEY_PRESS | KEY_CHANGE) == getKeyState(window, GLFW_KEY_T)) p->y -= 1;
    if ((KEY_PRESS | KEY_CHANGE) == getKeyState(window, GLFW_KEY_G)) p->y += 1;
    if ((KEY_PRESS | KEY_CHANGE) == getKeyState(window, GLFW_KEY_F)) p->x -= 1;
    if ((KEY_PRESS | KEY_CHANGE) == getKeyState(window, GLFW_KEY_H)) p->x += 1;

    animate(p->model, p->actualModel, p->state, p->time);
}
