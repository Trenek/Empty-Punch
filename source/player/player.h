#include "state.h"

typedef float vec3[3];

struct Grip {
    struct Entity *hex;

    int height;
    int width;
};

struct player {
    struct Entity *model;
    struct actualModel *actualModel;

    struct Grip *grip;

    int movements[4];

    int x;
    int y;

    float time;
    int state;
};

enum animation {
    CHARGE,
    DAMAGE_HIT,
    DAMAGE_KICK,
    LEFT_KICK,
    LEFT_LOW_PUNCH,
    LEFT_HIGH_PUNCH,
    RIGHT_KICK,
    RIGHT_LOW_PUNCH,
    RIGHT_HIGH_PUNCH,
    STANDING,
    WALK,

    FAST_WALK,
};

struct WindowManager;
void movePlayer(struct player *p, struct WindowManager *window, enum state *state, float deltaTime);
