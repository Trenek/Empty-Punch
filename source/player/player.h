#include "state.h"

typedef float vec3[3];

struct player {
    struct Entity *model;
    struct actualModel *actualModel;

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
void movePlayer(struct player *p, struct WindowManager *wc, float deltaTime, enum state *state);
void posePlayer(struct player *p, float deltaTime);
