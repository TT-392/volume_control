#include <stdio.h>
#include <assert.h>
#include "pico/stdlib.h"
#include "defines.h"
#include "monitor_matrix.h"
#include "usb.h"

#define printf cdc_printf

static const uint64_t timeout_us = 1000;
static bool matrix[5][8] = {0};
static uint64_t timestamp_last_update_us[5][8] = {0};

static volatile matrix_event_t matrix_event = {};
static _Atomic bool matrix_event_data_present = false;

static int queue_index = 0;
#define queue_size 30
static matrix_event_t queue[queue_size] = {};

bool matrix_monitor_event_available() {
    return matrix_event_data_present;
}

matrix_event_t matrix_monitor_get_event() {
    assert(matrix_event_data_present == true);

    matrix_event_t retval = matrix_event;
    matrix_event_data_present = false;

    return retval;
}

inline static void queue_put(matrix_event_t event) {
    if (queue_index < queue_size) {
        queue[queue_index] = event;
        queue_index++;
    }
}

inline static void queue_handle() {
    if (!matrix_event_data_present) {
        if (queue_index != 0) {
            queue_index--;
            matrix_event = queue[queue_index];
            matrix_event_data_present = true;
        }
    }
}

inline static void event_queue(int row, int col, bool key_state) {
    static matrix_event_t event;


    event.row = row;
    event.col = col;

    if (key_state == true)
        event.action = ACTION_DOWN;
    else
        event.action = ACTION_UP;

    if (matrix_event_data_present) {
        queue_put(event);
    } else {
        matrix_event = event;
        matrix_event_data_present = true;
    }
}

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

                if (key_state != matrix[r][c]) {
                    if (current_time_us - timestamp_last_update_us[r][c] > timeout_us) {
                        matrix[r][c] = key_state;
                        timestamp_last_update_us[r][c] = current_time_us;

                        event_queue(r, c, key_state);
                    }
                }
            }
            gpio_put(defines.rows[r], 1);


            sleep_us(1);
        }

        queue_handle();
    }
}

