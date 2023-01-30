#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/multicore.h"
#include "monitor_keys.h"
#include "keymap.h"

void print_key_matrix(_Atomic bool key_matrix[5][7]) {
    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 7; x++) {
            if (key_matrix[y][x])
                printf("%c ", keymap_left[y][x]);
            else
                printf("  ");
        }
        printf("\n");
    }
    printf("\n");
}

void key_matrix_to_events(_Atomic bool key_matrix[5][7]) {
    static bool last_matrix[5][7] = {};

    for (int y = 0; y < 5; y++) {
        for (int x = 0; x < 7; x++) {
            if (last_matrix[y][x] == 0 && key_matrix[y][x] == 1) {
                printf("%c", keymap_left[y][x]);
            }
            last_matrix[y][x] = key_matrix[y][x];
        }
    }
}

int main() {
    stdio_init_all();

    while (!stdio_usb_connected()) {
        sleep_ms(500);
    }
    printf("USB connected\n");

    multicore_launch_core1(monitor_keys);


    while (1) {
        key_matrix_to_events(key_matrix);
        //if (gpio_get(11)) {
        //    printf("L\n");
        //    reset_usb_boot(0, 0);
        //}
    }
}

