#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "matrix.h"

int main() {
    stdio_init_all();

    while (!stdio_usb_connected());
    printf("USB connected\n");
    gpio_pull_down(13);
    gpio_set_dir(13, false);
    gpio_init(9);
    gpio_set_dir(9, true);
    gpio_put(9, true);

    while (1) {
        if (gpio_get(13)) {
            printf("F\n");
        }
    }
 
//    while (1) {
//        char display[10][72] = {0};
//
//        for (int y = 0; y < 10; y++) {
//            for (int x = 0; x < 72; x++) {
//                display[y][x] = (((x / 5) % 2) ^ ((y / 5) % 2)) * 0x07;
//            }
//        }
//
//        matrix_put(display);
//        matrix_update();
//        sleep_ms(1000);
//
//        memset(display, 0x0, sizeof(display));
//        matrix_put(display);
//        matrix_update();
//
//        sleep_ms(1000);
//    }
//
//
//    while (1);

    while (1) {
        matrix_set_pixel(5, 5, 0xfe);
        sleep_ms(1000);
        matrix_fill_rect(0, 0, 5, 5, 0xfe);
        sleep_ms(1000);
        matrix_clear();
        matrix_fill_rect(0, 0, 0x47, 9, 0xfe);
        sleep_ms(1000);
        matrix_clear();
        sleep_ms(1000);
    }

}

