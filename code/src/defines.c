#include "defines.h"
#include "hamming.h"
#include "tusb.h"

static side_t left_side = {
    {11, 12, 13, 14, 15},
    5,
    {3, 4, 5, 6, 8, 9, 10},
    7,
    {{HID_KEY_ESCAPE, HID_KEY_GRAVE, HID_KEY_1, HID_KEY_2, HID_KEY_3, HID_KEY_4, HID_KEY_5, 0},
     {SPECIAL_KEY_0, HID_KEY_TAB, HID_KEY_Q, HID_KEY_W, HID_KEY_E, HID_KEY_R, HID_KEY_T, 0},
     {HID_KEY_BACKSLASH, HID_KEY_GUI_LEFT, HID_KEY_A, HID_KEY_S, HID_KEY_D, HID_KEY_F, HID_KEY_G, 0},
     {0, HID_KEY_SHIFT_LEFT, HID_KEY_Z, HID_KEY_X, HID_KEY_C, HID_KEY_V, HID_KEY_B, 0},
     {0, HID_KEY_CONTROL_LEFT, HID_KEY_GUI_LEFT, HID_KEY_ALT_LEFT, HID_KEY_6, HID_KEY_SPACE, HID_KEY_DELETE, 0}},
    .led0 = 18,
    .led1 = 19,
    .uart_tx = 24,
    .uart_rx = 25,
    .uart = uart1
};

static side_t right_side = {
    {29, 27, 20, 16, 17},
    5,
    {21, 22, 24, 19, 25, 18, 26, 28},
    8,
    {{0, HID_KEY_7, HID_KEY_8, HID_KEY_9, HID_KEY_0, HID_KEY_MINUS, HID_KEY_EQUAL, HID_KEY_BACKSPACE},
     {HID_KEY_Y, HID_KEY_U, HID_KEY_I, HID_KEY_O, HID_KEY_P, HID_KEY_BRACKET_LEFT, HID_KEY_BRACKET_RIGHT, HID_KEY_BACKSLASH},
     {HID_KEY_H, HID_KEY_J, HID_KEY_K, HID_KEY_L, HID_KEY_SEMICOLON, HID_KEY_APOSTROPHE, 0, HID_KEY_ENTER},
     {HID_KEY_N, HID_KEY_M, HID_KEY_COMMA, HID_KEY_PERIOD, HID_KEY_SLASH, HID_KEY_SHIFT_RIGHT, 0, 0},
     {HID_KEY_SPACE, 0, HID_KEY_ALT_RIGHT, 0, HID_KEY_GUI_RIGHT, 0, 0, 0}},
    .led0 = 12,
    .led1 = 13,
    .uart_tx = 0,
    .uart_rx = 1,
    .uart = uart0
};


static side_t active_side;
static bool initialized = false;

side_t get_defines() {
    if (!initialized) {
        gpio_init(PIN_SIDE_DETECT);
        gpio_set_dir(PIN_SIDE_DETECT, false);

        if (gpio_get(PIN_SIDE_DETECT) == LEFT_SIDE) {
            active_side = left_side;
        } else {
            active_side = right_side;
        }
        initialized = true;
    }
    return active_side;
}

