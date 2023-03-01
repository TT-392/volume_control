#include <stdint.h>
#include "tusb.h"
#include "process_input.h"
#include "defines.h"
#include "send_over_hid.h"

#define printf cdc_printf
#define VIM_KEY SPECIAL_KEY_0

enum mode {
    MODE_NORMAL,
    MODE_VIM
};


enum mode current_mode = MODE_NORMAL;


void vim_process_input(enum actions action, uint8_t key) {
    static bool shift_down = false;

    switch (key) {
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
            send_over_hid(action, key);
    }
}

void process_input(enum actions action, uint8_t key) {
    if (current_mode == MODE_NORMAL) {
        if (key < 0xf0)
            send_over_hid(action, key);
        else if (key == VIM_KEY && action == ACTION_DOWN)
            current_mode = MODE_VIM;

    } else if (current_mode == MODE_VIM) {
        if (key < 0xf0)
            vim_process_input(action, key);
    }
}
