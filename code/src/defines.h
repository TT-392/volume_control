#pragma once

#include "pico/stdlib.h"
#define PIN_SIDE_DETECT 23
#define LEFT_SIDE true
#define RIGHT_SIDE false

typedef struct side {
    uint rows[8];
    int row_count;
    uint cols[8];
    int col_count;
    char keymap[5][8];
    uint led0;
    uint led1;
    uint uart_tx;
    uint uart_rx;
    uart_inst_t *uart;
} side_t;

side_t get_defines();

