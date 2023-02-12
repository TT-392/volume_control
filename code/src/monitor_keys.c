#include <stdio.h>
#include "pico/stdlib.h"
#include "defines.h"

const uint64_t timeout_us = 1000;
_Atomic bool key_matrix[5][8] = {0};
uint64_t timestamp_last_update_us[5][8] = {0};

void monitor_keys() {
    side_t defines = get_defines();

    printf("start key monitoring\n");
    
    for (int i = 0; i < defines.col_count; i++) {
        gpio_init(defines.cols[i]);
        gpio_pull_up(defines.cols[i]);
        gpio_set_dir(defines.cols[i], 0);
    }

    for (int i = 0; i < defines.row_count; i++) {
        gpio_init(defines.rows[i]);
        gpio_set_dir(defines.rows[i], 1);
        gpio_put(defines.rows[i], 0);
    }


    while (1) {
        for (int r = 0; r < defines.row_count; r++) {
            gpio_put(defines.rows[r], 0);

            sleep_us(1);

            uint64_t current_time_us = to_us_since_boot(get_absolute_time());

            for (int c = 0; c < defines.col_count; c++) {
                bool key_state = !gpio_get(defines.cols[c]);

                if (key_state != key_matrix[r][c]) {
                    if (current_time_us - timestamp_last_update_us[r][c] > timeout_us) {
                        key_matrix[r][c] = key_state;
                        timestamp_last_update_us[r][c] = current_time_us;
                    }
                }
            }
            gpio_put(defines.rows[r], 1);


            sleep_us(1);
        }
    }
}

