#include <string.h>

#include "player.h"
#include "entity.h"

#include "instanceBuffer.h"
#include "windowManager.h"
#include "actualModel.h"

bool checkForColisionToAdd(struct player *p, const char *name, vec3 toAdd);
bool checkForColision(struct player *p, const char *name);

void animate(struct Entity *model, struct actualModel *actualModel, size_t animID, float deltaTime);

struct Grip createGrip(struct Grip a) {
    a.array = malloc(sizeof(float) * a.height * a.width);

    for (int i = 0; i < a.height * a.width; i += 1) {
        a.array[i] = 2.0;
    }

    return a;
}

void freeGrip(struct Grip a) {
    free(a.array);
}

void movePlayer(struct player *p, struct WindowManager *window, enum state *, float deltaTime) {
    struct instance *player = p->model->instance;
    struct instance *hex = p->grip->hex->instance;

    p->time += deltaTime;

    int index = p->x + p->grip->width * p->y;
    if ((p->x >= p->grip->width - (1 == p->y % 2)) ||
        (p->y >= p->grip->height) ||
        (p->y < 0) ||
        (p->x < 0)) {
        player->pos[0] = - (p->x - (p->grip->width - 1) / 2.0) * sqrt(3) - sqrt(3) * (p->y % 2) / 2;
        player->pos[1] = 1.5f * (p->y - (p->grip->height - 1) / 2.0);
        player->pos[2] -= 3 * deltaTime;
    }
    else if (hex[index].pos[2] < -10) {
        player->pos[2] -= 3 * deltaTime;
    }
    else {
        if ((KEY_PRESS | KEY_CHANGE) == getKeyState(window, p->movements[0])) p->y -= 1;
        if ((KEY_PRESS | KEY_CHANGE) == getKeyState(window, p->movements[1])) p->y += 1;
        if ((KEY_PRESS | KEY_CHANGE) == getKeyState(window, p->movements[2])) p->x -= 1;
        if ((KEY_PRESS | KEY_CHANGE) == getKeyState(window, p->movements[3])) p->x += 1;

        memcpy(player->pos, hex[index].pos, sizeof(float [3]));
        p->grip->array[index] -= deltaTime;
        if (p->grip->array[index] < 0) {
            hex[index].pos[2] = -100;
        }
        else if (p->grip->array[index] < 1) {
            hex[index].textureInc = 2;
        }

    }
        
    animate(p->model, p->actualModel, p->state, p->time);
}
