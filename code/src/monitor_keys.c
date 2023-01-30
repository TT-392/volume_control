#include <stdio.h>
#include "pico/stdlib.h"
#include "pins_left.h"

_Atomic bool key_matrix[5][7] = {0};

void monitor_keys() {
    uint cols[] = {PIN_COL0, PIN_COL1, PIN_COL2, PIN_COL3, PIN_COL4, PIN_COL5, PIN_COL6};
    uint rows[] = {PIN_ROW0, PIN_ROW1, PIN_ROW2, PIN_ROW3, PIN_ROW4};

    printf("start key monitoring\n");
    
    for (int i = 0; i < sizeof(cols) / sizeof(uint); i++) {
        gpio_init(cols[i]);
        gpio_pull_up(cols[i]);
        gpio_set_dir(cols[i], 0);
    }

    for (int i = 0; i < sizeof(rows) / sizeof(uint); i++) {
        gpio_init(rows[i]);
        gpio_set_dir(rows[i], 1);
        gpio_put(rows[i], 0);
    }


    while (1) {
        for (int r = 0; r < sizeof(rows) / sizeof(uint); r++) {
            gpio_put(rows[r], 0);

            sleep_us(1);
            for (int c = 0; c < sizeof(cols) / sizeof(uint); c++) {
                if (!gpio_get(cols[c])) {
                    key_matrix[r][c] = 1;
                } else {
                    key_matrix[r][c] = 0;
                }
            }
            gpio_put(rows[r], 1);


            sleep_us(1);
        }
    }
}

