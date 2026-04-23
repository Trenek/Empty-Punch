#include <cglm/cglm.h>
#include <GLFW/glfw3.h>

#include "button.h"
#include "defaultCamera.h"

#include "graphicsSetup.h"
#include "windowManager.h"

#include "entity.h"
#include "shadowInstance.h"

#include "Vertex.h"

#define MAX(x, y) ((x) > (y) ? (x) : (y)) 
#define MIN(x, y) ((y) > (x) ? (x) : (y))

#define MAX_4(x, y, z, h, i) MAX(MAX(x[i], y[i]), MAX(z[i], h[i])) 
#define MIN_4(x, y, z, h, i) MIN(MIN(x[i], y[i]), MIN(z[i], h[i]))

static void normalShadowButton(struct GraphicsSetup gs, struct WindowManager wm, struct Button *button) {
    vec3 p; {
        double pp[2];

        glfwGetCursorPos(wm.window, pp, pp + 1);

        p[0] = 2 * pp[0] / gs.swapChain.extent.width - 1;
        p[1] = 2 * pp[1] / gs.swapChain.extent.height - 1;
    }

    vec3 temp[4] = {
        { -1.0f, -1.0f, 0.0f },
        { 1.0f, 1.0f, 0.0f },
        { -1.0f, 1.0f, 0.0f },
        { 1.0f, -1.0f, 0.0f },
    };

    button->chosen = -1;
    for (int i = 0; i < button->qButton; i += 1) {
        vec3 temp2[4]; {
            mat4 tempMat; {
                glm_mat4_mul(button->camera->proj, ((struct shadowInstanceBuffer *)button->entity[i]->buffer[0])->modelMatrix, tempMat);
            }
            glm_mat4_mulv3(tempMat, temp[0], 1, temp2[0]);
            glm_mat4_mulv3(tempMat, temp[1], 1, temp2[1]);
            glm_mat4_mulv3(tempMat, temp[2], 1, temp2[2]);
            glm_mat4_mulv3(tempMat, temp[3], 1, temp2[3]);
        }

        float left = MIN_4(temp2[0], temp2[1], temp2[2], temp2[3], 0);
        float right = MAX_4(temp2[0], temp2[1], temp2[2], temp2[3], 0);
        float down = MIN_4(temp2[0], temp2[1], temp2[2], temp2[3], 1);
        float up = MAX_4(temp2[0], temp2[1], temp2[2], temp2[3], 1);

        if (((struct shadowInstance *)button->entity[i]->instance)->shadow = p[0] > left && p[0] < right && p[1] > down && p[1] < up) {
            button->chosen = i;
        }
    }

    button->isClicked = (KEY_PRESS | KEY_CHANGE) == getMouseState(&wm, GLFW_MOUSE_BUTTON_LEFT) && button->chosen >= 0;
}

#define JP(x) GLFW_PRESS == state.buttons[x] && wm.data->gamepad[x] != state.buttons[x]
#define JS(x) 0.5 < state.axes[x] && 0.5 > wm.data->state[x]
#define JR(x) -0.5 > state.axes[x] && -0.5 < wm.data->state[x]
#define ST(x) wm.data->gamepad[x] = state.buttons[x]
#define SS(x) wm.data->state[x] = state.axes[x]

void gamepadShadowButton(struct WindowManager wm, struct Button *button) {
    GLFWgamepadstate state;

    if (glfwGetGamepadState(button->joystick, &state)) {
        if (
            JP(GLFW_GAMEPAD_BUTTON_DPAD_UP) ||
            JR(GLFW_GAMEPAD_AXIS_LEFT_Y)
        ) button->chosen -= 1;
        if (
            JP(GLFW_GAMEPAD_BUTTON_DPAD_DOWN) ||
            JS(GLFW_GAMEPAD_AXIS_LEFT_Y)
        ) button->chosen += 1;

        if (button->chosen < 0) button->chosen = button->qButton - 1;
        if (button->chosen >= button->qButton) button->chosen = 0;

        for (int i = 0; i < button->qButton; i += 1) {
            ((struct shadowInstance *)button->entity[i]->instance)->shadow = 0;
        }

        ((struct shadowInstance *)button->entity[button->chosen]->instance)->shadow = 1;

        button->isClicked = JP(GLFW_GAMEPAD_BUTTON_A);

        SS(GLFW_GAMEPAD_AXIS_LEFT_Y);
        ST(GLFW_GAMEPAD_BUTTON_DPAD_UP);
        ST(GLFW_GAMEPAD_BUTTON_DPAD_DOWN);
        ST(GLFW_GAMEPAD_BUTTON_A);
    }
}

void shadowButton(struct GraphicsSetup gs, struct WindowManager wm, struct Button *button) {
    if (glfwJoystickIsGamepad(button->joystick)) {
        gamepadShadowButton(wm, button);
    }
    else {
        normalShadowButton(gs, wm, button);
    }
}
