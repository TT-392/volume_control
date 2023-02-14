#include <stdlib.h>
#include <string.h>
#include "tusb.h"
#include "tinyusb/hid.h"

int main() {
    hid_init();

    while (1) {
        uint64_t current_time_ms = to_ms_since_boot(get_absolute_time());
        bool keys_pressed = (current_time_ms / 1000) % 2;
        keys_pressed = false;

        if (keys_pressed) {
            uint8_t key_codes[8] = {HID_KEY_A, HID_KEY_B};
            hid_update(key_codes);
        } else {
            uint8_t key_codes[8] = {0};
            hid_update(key_codes);
        }

        hid_task();
    }

    return 0;
}
