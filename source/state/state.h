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

enum resourceEnum {
    TEXTURES = 0,
    MODELS,
    RENDER_PASS_CORE,
    OBJECT_LAYOUT,
    GRAPHIC_PIPELINE_LAYOUTS,
    GRAPHIC_PIPELINES,
    SOUND_MANAGER,
    SCENE_INFO,
    ENTITY,
    SCREEN_DATA,
    COMMAND_QUEUE,

    TEXTURE_COLOR = 0,
    TEXTURE_CUBEMAP,

    MODEL_PLAYER = 0,
    MODEL_HEX,
    MODEL_FONT,
    MODEL_FLAT,
    MODEL_SKYBOX,

    RENDER_PASS_CLEAN = 0,
    RENDER_PASS_STAY,

    OBJECT_LAYOUT_CAMERA = 0,
    OBJECT_LAYOUT_GLTF,
    OBJECT_LAYOUT_OBJ,
    OBJECT_LAYOUT_REC,
    OBJECT_LAYOUT_FONT,

    GRAPHICS_PIPELINE_LAYOUT_GLTF = 0,
    GRAPHICS_PIPELINE_LAYOUT_OBJ_CUBEMAP,
    GRAPHICS_PIPELINE_LAYOUT_REC,
    GRAPHICS_PIPELINE_LAYOUT_FONT,

    GRAPHIC_PIPELINE_FLOOR = 0,
    GRAPHIC_PIPELINE_ANIM,
    GRAPHIC_PIPELINE_TEXT,
    GRAPHIC_PIPELINE_BUTTON,
    GRAPHIC_PIPELINE_SKYBOX,

    SCENE_HEIGHT = 0,
    SCENE_WIDTH,

    ENTITY_HEX = 0,
    ENTITY_PLAYER_1,
    ENTITY_PLAYER_2,
    ENTITY_TEXT_MAIN_MENU,
    ENTITY_TEXT_PLAY,
    ENTITY_TEXT_EXIT,
    ENTITY_FLAT_1,
    ENTITY_FLAT_2,
    ENTITY_BACKGROUND,

    SCREEN_BASE = 0,
    SCREEN_SKYBOX,
    SCREEN_INTERFACE,

    COMMAND_QUEUE_GRAPHICS = 0,
};

struct EngineCore;
struct renderPassObj;

void game(struct EngineCore *engine, enum state *state);
void loadResources(struct EngineCore *engine, enum state *state);
void loadGame(struct EngineCore *engine, enum state *state);
void menu(struct EngineCore *engine, enum state *state);
void end(struct EngineCore *engine, enum state *state);

#endif
