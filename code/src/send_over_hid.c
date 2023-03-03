#include <stdbool.h>
#include "tusb.h"
#include "usb.h"
#include "defines.h"

#define printf cdc_printf

#define MAX_DEPRESSED_KEYS 6

void send_over_hid(enum actions action, uint8_t key) {
    static uint8_t depressed_keys[MAX_DEPRESSED_KEYS] = {};
    static uint8_t modifiers = 0;

    if ((key & 0xf0) == 0xe0) { // modifier keys
        uint8_t bit = 1 << (key & 0x0f);

        if (action == ACTION_DOWN) {
            if (modifiers & bit) {
                printf("SEND OVER HID: modifier 0x%02x received a down event but was already pressed\r\n\r", key);
                printf("\n");
                usb_task();
            }

            modifiers |= bit;
        } else if (action == ACTION_UP) {
            if (modifiers & bit == 0) {
                printf("SEND OVER HID: modifier 0x%02x received an up event but wasn't pressed down\r\n\r", key);
                printf("\n");
                usb_task();
            }

            modifiers &= ~bit;
        } else return;
    } else {
        if (action == ACTION_DOWN) {
            for (int i = 0; i < MAX_DEPRESSED_KEYS; i++) {
                if (depressed_keys[i] == key) {
                    printf("SEND OVER HID: Key 0x%02x received a down event, but was already pressed\r\n\r", key);
                    printf("\n");
                    usb_task();
                }
            }

            for (int i = 0; i < MAX_DEPRESSED_KEYS; i++) {
                if (depressed_keys[i] == 0) {
                    depressed_keys[i] = key;
                    break;
                }
            }
        } else if (action == ACTION_UP) {
            bool handled_release = false;

            for (int i = 0; i < MAX_DEPRESSED_KEYS; i++) {
                if (depressed_keys[i] == key) {
                    depressed_keys[i] = 0;
                    handled_release = true;
                    break;
                }
            }

            if (!handled_release) {
                if (key != HID_KEY_I) {
                    printf("SEND OVER HID: Key 0x%02x received an up event, but wasn't pressed down\r\n\r", key);
                    printf("\n");
                    usb_task();
                }
            }
        } else return;
    }

    
    keyboard_update(modifiers, depressed_keys);
}

