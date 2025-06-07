#include <string.h>

#include "player.h"
#include "entity.h"

#include "instanceBuffer.h"
#include "windowManager.h"
#include "actualModel.h"

bool checkForColisionToAdd(struct player *p, const char *name, vec3 toAdd);
bool checkForColision(struct player *p, const char *name);

void animate(struct Entity *model, struct actualModel *actualModel, size_t animID, float deltaTime);

void movePlayer(struct player *p, struct WindowManager *window, enum state *state, float deltaTime) {
    struct instance *player = p->model->instance;
    struct instance *hex = p->hex->instance;

    p->time += deltaTime;

    if ((KEY_PRESS | KEY_CHANGE) == getKeyState(window, p->movements[0])) p->y -= 1;
    if ((KEY_PRESS | KEY_CHANGE) == getKeyState(window, p->movements[1])) p->y += 1;
    if ((KEY_PRESS | KEY_CHANGE) == getKeyState(window, p->movements[2])) p->x -= 1;
    if ((KEY_PRESS | KEY_CHANGE) == getKeyState(window, p->movements[3])) p->x += 1;

    int index = p->x + p->width * p->y;

    memcpy(player->pos, hex[index].pos, sizeof(float [3]));

    if (p->x >= p->width - (1 == p->y % 2)) *state = EXIT;
    if (p->y >= p->height) *state = EXIT;
    if (p->y < 0) *state = EXIT;
    if (p->x < 0) *state = EXIT;
        
    animate(p->model, p->actualModel, p->state, p->time);
}
