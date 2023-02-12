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
    uint led1;
    uint led2;
} side_t;

side_t get_defines();

