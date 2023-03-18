#include <stdint.h>
#include "tusb.h"
#include "process_input.h"
#include "defines.h"
#include "usb.h"
#include "send_over_hid.h"

#define printf cdc_printf
#define VIM_KEY SPECIAL_KEY_0
#define FUNCTION_KEY SPECIAL_KEY_1

enum mode {
    MODE_NORMAL,
    MODE_VIM,
    MODE_FUNCTION
};

enum modifiers {
    MODIFIER_SHIFT = 1 << 8,
    MODIFIER_CTRL = 1 << 9
};

enum mode current_mode = MODE_NORMAL;
enum modifiers current_modifiers = 0;

void function_process_input(enum actions action, uint8_t key) {
    const uint8_t keymap_normal[] = {HID_KEY_1,  HID_KEY_2,  HID_KEY_3,  HID_KEY_4,  HID_KEY_5,  HID_KEY_6,  HID_KEY_7,  HID_KEY_8,  HID_KEY_9,  HID_KEY_0,   HID_KEY_MINUS, HID_KEY_EQUAL};
    const uint8_t keymap_fn[] =     {HID_KEY_F1, HID_KEY_F2, HID_KEY_F3, HID_KEY_F4, HID_KEY_F5, HID_KEY_F6, HID_KEY_F7, HID_KEY_F8, HID_KEY_F9, HID_KEY_F10, HID_KEY_F11,   HID_KEY_F12};

    for (int i = 0; i < sizeof(keymap_normal); i++) {
        if (key == keymap_normal[i]) {
            send_over_hid(action, keymap_fn[i]);
            return;
        }
    }

    send_over_hid(action, key);
}

void vim_process_input(enum actions action, uint16_t key) {
    static bool shift_down = false;

    switch (key) {
        case HID_KEY_0:
            send_over_hid(action, HID_KEY_HOME);
            break;

        case HID_KEY_4 | MODIFIER_SHIFT: // $
            send_over_hid(action, HID_KEY_END);
            break;
            
        case HID_KEY_H | MODIFIER_SHIFT:
            send_over_hid(action, HID_KEY_PAGE_UP);
            break;

        case HID_KEY_L | MODIFIER_SHIFT:
            send_over_hid(action, HID_KEY_PAGE_DOWN);
            break;

        case HID_KEY_H:
            send_over_hid(action, HID_KEY_ARROW_LEFT);
            break;
            
        case HID_KEY_J:
            send_over_hid(action, HID_KEY_ARROW_DOWN);
            break;

        case HID_KEY_K:
            send_over_hid(action, HID_KEY_ARROW_UP);
            break;

        case HID_KEY_L:
            send_over_hid(action, HID_KEY_ARROW_RIGHT);
            break;

        case HID_KEY_X:
            send_over_hid(action, HID_KEY_DELETE);

            if (shift_down) {
                send_over_hid(ACTION_UP, HID_KEY_SHIFT_LEFT);
                shift_down = false;
            }
            break;

        case HID_KEY_D:
            send_over_hid(action, HID_KEY_DELETE);

            if (shift_down) {
                send_over_hid(ACTION_UP, HID_KEY_SHIFT_LEFT);
                shift_down = false;
            }
            break;

        case HID_KEY_V:
            if (action == ACTION_DOWN) {
                if (shift_down) {
                    send_over_hid(ACTION_UP, HID_KEY_SHIFT_LEFT);
                    shift_down = false;
                } else {
                    send_over_hid(ACTION_DOWN, HID_KEY_SHIFT_LEFT);
                    shift_down = true;
                }
            }
            break;

        case HID_KEY_I:
            if (shift_down) {
                send_over_hid(ACTION_UP, HID_KEY_SHIFT_LEFT);
                shift_down = false;
            }
            current_mode = MODE_NORMAL;
            break;

        default:
            key &= 0xff;

            if (key != HID_KEY_SHIFT_LEFT &&
                key != HID_KEY_SHIFT_RIGHT &&
                key != HID_KEY_CONTROL_LEFT &&
                key != HID_KEY_CONTROL_RIGHT) {

                printf("0x%04x\n");
                send_over_hid(action, key);
            }
    }
}

void process_input(enum actions action, uint8_t key) {
    if (key == HID_KEY_SHIFT_LEFT || key == HID_KEY_SHIFT_RIGHT) {
        if (action == ACTION_DOWN)
            current_modifiers |= MODIFIER_SHIFT;

        else if (action == ACTION_UP)
            current_modifiers &= ~MODIFIER_SHIFT;
    }

    if (key == HID_KEY_CONTROL_LEFT || key == HID_KEY_CONTROL_RIGHT) {
        if (action == ACTION_DOWN)
            current_modifiers |= MODIFIER_CTRL;

        else if (action == ACTION_UP)
            current_modifiers &= ~MODIFIER_CTRL;
    }



    if (current_mode == MODE_NORMAL) {
        if (key < 0xf0)
            send_over_hid(action, key);
        else if (key == VIM_KEY && action == ACTION_DOWN)
            current_mode = MODE_VIM;
        else if (key == FUNCTION_KEY && action == ACTION_DOWN)
            current_mode = MODE_FUNCTION;

    } else if (current_mode == MODE_VIM) {
        if (key < 0xf0)
            vim_process_input(action, key | current_modifiers);

    } else if (current_mode == MODE_FUNCTION) {
        if (key < 0xf0)
            function_process_input(action, key);
        else if (key == FUNCTION_KEY && action == ACTION_DOWN)
            current_mode = MODE_NORMAL;
    }
}
