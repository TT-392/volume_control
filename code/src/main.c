#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
#include "hamming.h"
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/multicore.h"
#include "monitor_matrix.h"
#include "process_input.h"
#include "defines.h"
#include "trrs.h"
#include "usb.h"
#define printf cdc_printf

int main() {
    usb_init();

    uint8_t pressed_keys[8] = {};
    gpio_init(25);
    gpio_set_dir(25, 1);
    gpio_put(25, 1);


    gpio_init(18);
    gpio_pull_up(18);
    gpio_set_dir(18, 0);
    gpio_init(17);
    gpio_pull_up(17);
    gpio_set_dir(17, 0);
    gpio_init(16);
    gpio_pull_up(16);
    gpio_set_dir(16, 0);


    bool last_state = gpio_get(16);

    int i = 0;
    bool clear = false;


    while (true) {
        bool new_state = gpio_get(16);

        if (last_state != new_state) {
            if (last_state == 0 && new_state == 1) {
                if (new_state != gpio_get(17)) {
                    keyboard_update(0, (uint8_t[6]){HID_KEY_VOLUME_DOWN});
                } else {
                    keyboard_update(0, (uint8_t[6]){HID_KEY_VOLUME_UP});
                }
            }

            sleep_ms(10);
            keyboard_update(0, (uint8_t[6]){0});
        }

        if (!gpio_get(18)) {
            keyboard_update(0, (uint8_t[6]){HID_KEY_MUTE});
            sleep_ms(100);
            clear = true;
        } else if (clear) {
            clear = false;
            keyboard_update(0, (uint8_t[6]){0});
        }

        last_state = new_state;





        //bool master = usb_connected();
        //
        //if (matrix_monitor_event_available()) {
        //    matrix_event_t matrix_event = matrix_monitor_get_event();
        //    uint8_t mapped_key = defines.keymap[matrix_event.row][matrix_event.col];

        //    if (master) {
        //        process_input(matrix_event.action, mapped_key);

        //    } else {
        //        trrs_send(hamming_17_11_encode(0xffff & (matrix_event.action | mapped_key)));
        //    }

        //}

        //if (master) {
        //    if (trrs_data_available()) {
        //        uint17_t packet = trrs_read();
        //        packet = hamming_17_11_decode(packet);

        //        if (packet != 0) {
        //            process_input(packet & 0xff00, packet & 0xff);
        //        }
        //    }
        //}

        usb_task();
   }
}

