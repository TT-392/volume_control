#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "tusb.h"
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
            }
        }


        //event_t event = key_matrix_to_events(key_matrix);
        //
        //if (master) {
        //    if (event.action != action_none) {
        //        process_input(event.action, event.key);
        //    }
        //    if (trrs_data_ready()) {
        //        uint16_t packet = trrs_read();

        //        event = (event_t){
        //            .key = packet & 0x0ff,
        //            .action = packet & 0xf000
        //        };

        //        process_input(event.action, event.key);
        //    }
        //} else {
        //    if (event.action != action_none)
        //        trrs_send(event.action | event.key);
        //}


        //if (cdc_data_available()) {
        //    char buff[101];
        //    cdc_get_line(buff);
        //    cdc_printf("aaa: %s\n\r", buff);
        //}

        usb_task();
   }
}

