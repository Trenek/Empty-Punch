﻿#include "engineCore.h"

#include "state.h"

int main() {
    struct EngineCore engine = setup("Empty Punch");
    void (* const state[])(struct EngineCore *engine, enum state *state) = {
        [GAME] = game,
        [END] = end,
        [LOAD_GAME] = loadGame,
        [MAIN_MENU] = menu,
        [LOAD_RESOURCES] = loadResources,
    };
    enum state stateID = LOAD_RESOURCES;

    do {
        state[stateID](&engine, &stateID);
    } while (stateID != EXIT && !shouldWindowClose(engine.window));

    cleanup(engine);

    return 0;
}
