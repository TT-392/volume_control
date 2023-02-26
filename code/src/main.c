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

int main() {
    usb_init();
    trrs_init();

    side_t defines = get_defines();

    uint8_t pressed_keys[8] = {};


    multicore_launch_core1(monitor_keys);

    while (true) {
        bool master = usb_connected();
        
        if (matrix_monitor_event_available()) {
            matrix_event_t matrix_event = matrix_monitor_get_event();
            uint8_t mapped_key = defines.keymap[matrix_event.row][matrix_event.col];

            if (master) {
                process_input(matrix_event.action, mapped_key);

            } else {
                trrs_send(hamming_16_11_encode(0xffff & (matrix_event.action | mapped_key)));
            }

        }

        if (master) {
            if (trrs_data_available()) {
                uint16_t packet = trrs_read();
                packet = hamming_16_11_decode(packet);

                if (packet != 0) {
                    process_input(packet & 0xff00, packet & 0xff);
                }
            }
        }

        usb_task();
   }
}

