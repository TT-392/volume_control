#include <stdint.h>
#include "tusb.h"
#include "usb.h"
#include "process_input.h"
#include "defines.h"

#define printf cdc_printf
#define MAX_DEPRESSED_KEYS 6
#define VIM_KEY SPECIAL_KEY_0

enum mode {
    MODE_NORMAL,
    MODE_VIM
};


enum mode current_mode = MODE_NORMAL;

void send_over_hid(enum actions action, uint8_t key) {
    static uint8_t depressed_keys[MAX_DEPRESSED_KEYS] = {};
    static uint8_t modifiers = 0;

    if ((key & 0xf0) == 0xe0) { // modifier keys
        uint8_t bit = 1 << (key & 0x0f);

        if (action == ACTION_DOWN) {
            modifiers |= bit;
        } else if (action == ACTION_UP) {
            modifiers &= ~bit;
        } else return;
    } else {
        if (action == ACTION_DOWN) {
            for (int i = 0; i < MAX_DEPRESSED_KEYS; i++) {
                if (depressed_keys[i] == 0) {
                    depressed_keys[i] = key;
                    break;
                }
            }
        } else if (action == ACTION_UP) {
            for (int i = 0; i < MAX_DEPRESSED_KEYS; i++) {
                if (depressed_keys[i] == key) {
                    depressed_keys[i] = 0;
                    break;
                }
            }
        } else return;
    }

//    cdc_printf("%02x, %02x, %02x, %02x, %02x, %02x\r\n", depressed_keys[0], depressed_keys[1], depressed_keys[2], depressed_keys[3], depressed_keys[4], depressed_keys[5]);
    printf("modifiers: %i\r\n", modifiers);
    
    keyboard_update(modifiers, depressed_keys);
}

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
        vim_process_input(action, key);
    }
}
