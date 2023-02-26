#pragma once

#include "pico/stdlib.h"
#define PIN_SIDE_DETECT 23
#define LEFT_SIDE true
#define RIGHT_SIDE false

#define SPECIAL_KEY_0 0xf0
#define SPECIAL_KEY_1 0xf1
#define SPECIAL_KEY_2 0xf2
#define SPECIAL_KEY_3 0xf3
#define SPECIAL_KEY_4 0xf4

#define SPECIAL_KEY_0 0xf0
#define SPECIAL_KEY_1 0xf1
#define SPECIAL_KEY_2 0xf2
#define SPECIAL_KEY_3 0xf3
#define SPECIAL_KEY_4 0xf4

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

enum actions { // Should be smaller than 12 bits because hamming encoding
    ACTION_NONE = 0,
    ACTION_UP = 0x100,
    ACTION_DOWN = 0x200
};


side_t get_defines();

