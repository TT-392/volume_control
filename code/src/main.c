#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/multicore.h"
#include "monitor_keys.h"
#include "defines.h"
#include "trrs.h"

void print_key_matrix(_Atomic bool key_matrix[5][8]) {
    side_t defines = get_defines();

    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 8; x++) {
            if (key_matrix[y][x])
                printf("%c ", defines.keymap[y][x]);
            else
                printf("  ");
        }
        printf("\n");
    }
    printf("\n");
}

#define action_up 0x8000
#define action_down 0x4000
#define action_none 0

typedef struct event {
    uint8_t key;
    uint16_t action;
} event_t;

event_t key_matrix_to_events(_Atomic bool key_matrix[5][8]) {
    static bool last_matrix[5][8] = {};
    side_t defines = get_defines();

    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 8; x++) {
            if (last_matrix[y][x] == 0 && key_matrix[y][x] == 1) {
                last_matrix[y][x] = key_matrix[y][x];
                return (event_t){.key = defines.keymap[y][x], .action = action_down};
            } else if (last_matrix[y][x] == 1 && key_matrix[y][x] == 0) {
                last_matrix[y][x] = key_matrix[y][x];
                return (event_t){.key = defines.keymap[y][x], .action = action_up};
            }
            last_matrix[y][x] = key_matrix[y][x];
        }
    }

    return (event_t){.action = action_none};
}

int main() {
    stdio_init_all();
    side_t defines = get_defines();

    trrs_init();

    stdio_init_all();

    multicore_launch_core1(monitor_keys);

    while (true) {
        bool master = stdio_usb_connected();

        event_t event = key_matrix_to_events(key_matrix);
        
        if (master) {
            if (event.action != action_none) {
                //printf("%c %s\n", event.key, event.action == action_up ? "up" : "down");
                if (event.action == action_down) {
                    printf("%c", event.key);
                }
            }
            if (trrs_data_ready()) {
                uint16_t packet = trrs_read();

                event = (event_t){
                    .key = packet & 0x0ff,
                    .action = packet & 0xf000
                };

                if (event.action == action_down) {
                    printf("%c", event.key);
                }
                //printf("%c %s\n", event.key, event.action == action_up ? "up" : "down");
            }
        } else {
            if (event.action != action_none)
                trrs_send(event.action | event.key);
        }
    }
}

