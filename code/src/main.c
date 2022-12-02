#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include "pico/stdlib.h"
#include "pico/bootrom.h"
#include "pico/multicore.h"
#include "monitor_keys.h"

int main() {
    stdio_init_all();

    while (!stdio_usb_connected());
    printf("USB connected\n");

    multicore_launch_core1(monitor_keys);

    while (1) {
        //if (gpio_get(11)) {
        //    printf("L\n");
        //    reset_usb_boot(0, 0);
        //}
    }
}

