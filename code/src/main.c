#include <stdlib.h>
#include <string.h>
#include "pico/stdlib.h"
#include "tusb.h"
#include "tinyusb/usb.h"

int main() {
    usb_init();
    gpio_init(PICO_DEFAULT_LED_PIN);
    gpio_set_dir(PICO_DEFAULT_LED_PIN, GPIO_OUT);

    while (1) {
        uint64_t current_time_ms = to_ms_since_boot(get_absolute_time());
        bool keys_pressed = (current_time_ms / 1000) % 2;
        keys_pressed = false;

        if (keys_pressed) {
            uint8_t key_codes[8] = {HID_KEY_A, HID_KEY_B};
            keyboard_update(key_codes);
        } else {
            uint8_t key_codes[8] = {0};
            keyboard_update(key_codes);
        }

        usb_task();

        static int i = 0;
        if (cdc_connected()) {
            if (cdc_data_available()) {
                static bool done = false;
                if ((current_time_ms / 100) % 2) {
                    if (!done) {
                        cdc_printf("aaa: %i\n", i++);
                        done = true;
                    }
                } else {
                    done = false;
                }
            }
        }
    }

    return 0;
}
