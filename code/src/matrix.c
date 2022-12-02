#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "matrix.h"

#define START_BYTE          0xff
#define CMD_SET_PIXEL       0x18
#define CMD_CLEAR           0x00
#define CMD_FILL_DISPLAY    0x08
#define CMD_FILL_RECT       0x28

char current_display[10][72];
char new_display[10][72];

int written_bytes = 0;

void matrix_init() {
    uart_init(uart0, 115200);
    gpio_set_function(0, GPIO_FUNC_UART);

    matrix_clear();
    memset(current_display, 0, sizeof(current_display));
    memset(new_display, 0, sizeof(current_display));
}

static void find_and_write_block(int x, int y) {
    uint8_t brightness = new_display[y][x];
    matrix_set_pixel(x, y, brightness);
    //int x1 = x;

    //while (x < 72) {
    //    if (new_display[y][x] == brightness && new_display[y][x] != current_display[y][x]) {
    //        x++;
    //    } else {
    //        break;
    //    }
    //}

    //int x2 = x;

    //while (y < 10) {
    //    bool all_same_pixels = true;

    //    for (x = x1; x <= x2; x++) {
    //        if (!(new_display[y][x] == brightness && new_display[y][x] != current_display[y][x])) {
    //            all_same_pixels = false;
    //        }
    //    }

    //    if (all_same_pixels) {
    //        y++;
    //    } else {
    //        break;
    //    }
    //}

    matrix_set_pixel(x, y, brightness);
}

void matrix_update() {
    for (int y = 0; y < 10; y++) {
        for (int x = 0; x < 72; x++) {
            if (current_display[y][x] != new_display[y][x]) {

                find_and_write_block(x, y);
            }
        }
    }

    //printf("Bytecount: %i", written_bytes);
    written_bytes = 0;
}

void matrix_put(char display[10][72]) {
    memcpy(new_display, display, sizeof(new_display));
}

void matrix_clear() {
    uart_putc_raw(uart0, START_BYTE);
    uart_putc_raw(uart0, CMD_CLEAR);
}

void matrix_set_pixel(int x, int y, uint8_t brightness) {
    uart_putc_raw(uart0, START_BYTE);
    uart_putc_raw(uart0, CMD_SET_PIXEL);
    uart_putc_raw(uart0, x + 1); // Matrix coords start counting from 1
    uart_putc_raw(uart0, 10 - y);
    uart_putc_raw(uart0, brightness);

    current_display[y][x] = brightness;

    written_bytes += 5;
}

void matrix_fill(uint8_t brightness) {
    uart_putc_raw(uart0, START_BYTE);
    uart_putc_raw(uart0, CMD_FILL_DISPLAY);
    uart_putc_raw(uart0, brightness);
}

void matrix_fill_rect(int x1, int y1, int x2, int y2, uint8_t brightness) {
    uart_putc_raw(uart0, START_BYTE);
    uart_putc_raw(uart0, CMD_FILL_RECT);

    int width = x2 - x1 + 1;
    int height = y2 - y1 + 1;

    int matrix_x1 = x1 + 1;
    int matrix_y1 = 10 - y2;

    uart_putc_raw(uart0, matrix_x1);
    uart_putc_raw(uart0, matrix_y1);
    uart_putc_raw(uart0, width);
    uart_putc_raw(uart0, height);
    uart_putc_raw(uart0, brightness);

    for (int y = y1; y <= y2; y++) {
        memset(&current_display[y][x1], brightness, x2 - x1 + 1);
    }

    written_bytes += 7;
}
