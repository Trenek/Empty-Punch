#ifndef STATE_H
#define STATE_H

#include <stdint.h>

enum state {
    LOAD_RESOURCES,
    LOAD_GAME,
    MAIN_MENU,
    GAME,
    PAUSE,
    WIN_SCREEN,
    LOSE,
    END,
    EXIT
};

struct EngineCore;
struct renderPassObj;

void game(struct EngineCore *engine, enum state *state);
void loadResources(struct EngineCore *engine, enum state *state);
void loadGame(struct EngineCore *engine, enum state *state);
void menu(struct EngineCore *engine, enum state *state);
void end(struct EngineCore *engine, enum state *state);

#endif
