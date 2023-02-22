#include <stdint.h>
#include "usb.h"
#include "process_input.h"

#define MAX_DEPRESSED_KEYS 6

void process_input(enum actions action, uint8_t key) {
    static uint8_t depressed_keys[MAX_DEPRESSED_KEYS] = {};

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
    } else {
        return;
    }

    cdc_printf("%02x, %02x, %02x, %02x, %02x, %02x\r\n", depressed_keys[0], depressed_keys[1], depressed_keys[2], depressed_keys[3], depressed_keys[4], depressed_keys[5]);
    
    keyboard_update(depressed_keys);
}
