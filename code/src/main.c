#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/multicore.h"
#include "monitor_keys.h"
#include "process_input.h"
#include "defines.h"
#include "trrs.h"
#include "usb.h"

#define printf cdc_printf

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
    usb_init();
    trrs_init();

    side_t defines = get_defines();

    uint8_t pressed_keys[8] = {};


    multicore_launch_core1(monitor_keys);

    while (true) {
        if (key_monitor_event_available()) {
            key_event_t matrix_event = key_monitor_get_event();
            process_input(matrix_event.action, defines.keymap[matrix_event.row][matrix_event.col]);
        }

        //bool master = usb_connected();

        //event_t event = key_matrix_to_events(key_matrix);
        //
        //if (master) {
        //    if (event.action != action_none) {
        //        process_input(event.action, event.key);
        //    }
        //    if (trrs_data_ready()) {
        //        uint16_t packet = trrs_read();

        //        event = (event_t){
        //            .key = packet & 0x0ff,
        //            .action = packet & 0xf000
        //        };

        //        process_input(event.action, event.key);
        //    }
        //} else {
        //    if (event.action != action_none)
        //        trrs_send(event.action | event.key);
        //}


        //if (cdc_data_available()) {
        //    char buff[101];
        //    cdc_get_line(buff);
        //    cdc_printf("aaa: %s\n\r", buff);
        //}

        usb_task();
   }
}

