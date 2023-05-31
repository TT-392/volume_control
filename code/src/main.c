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

#define IR_SAMPLE_US 100
#define IR_SAMPLE_COUNT (800 / IR_SAMPLE_US)

enum rc5_a18_buttons {
    RC5_A18_BUTTON_HOME = 0x01,
    RC5_A18_BUTTON_VOL_DOWN = 0x03,
    RC5_A18_BUTTON_MENU = 0x05,
    RC5_A18_BUTTON_DOWN = 0x08,
    RC5_A18_BUTTON_INFO = 0x09,
    RC5_A18_BUTTON_POWER = 0x0C,
    RC5_A18_BUTTON_RIGHT = 0x0D,
    RC5_A18_BUTTON_UP = 0x10,
    RC5_A18_BUTTON_PAG_DOWN = 0x11,
    RC5_A18_BUTTON_VOL_UP = 0x13,
    RC5_A18_BUTTON_LEFT = 0x15,
    RC5_A18_BUTTON_BACK = 0x18,
    RC5_A18_BUTTON_PAG_UP = 0x1A,
    RC5_A18_BUTTON_OK = 0x20,
    RC5_A18_BUTTON_SEARCH = 0x21,
    RC5_A18_BUTTON_MUTE = 0x32,
    RC5_A18_BUTTON_STOP = 0x37
};

volatile struct {
    bool active;
    bool last;
    unsigned count;

    unsigned value;
    unsigned value_bits;
} ir;

bool irq_sample_ir(repeating_timer_t *rt) {
    bool new = gpio_get(28);

    // Activate
    if (!ir.active) {
        if (new == ir.last) return true;

        // Initialize
        ir.count = 0;
        ir.value = 0;
        ir.value_bits = 0;
        ir.active = true;
    }

    // Check for time out
    if (ir.count++ > IR_SAMPLE_COUNT * 4) {
        ir.active = false;
        return true;
    }

    // Don't do anything if it hasn't changed
    if (ir.last == new) return true;
    ir.last = new;

    // Ignore the change if it changed too fast
    if (ir.count < IR_SAMPLE_COUNT * 1.5) return true;

    // Sample the bit
    bool bit = !new;
    ir.value <<= 1;
    ir.value |= bit;
    ir.value_bits += 1;

    // Time for the next bit
    ir.count = 0;

    return true;
}
repeating_timer_t timer_sample_ir;

struct rc5_data {
    unsigned control;
    unsigned address;
    unsigned command;
};

struct rc5_data rc5_decode(unsigned val)
{
    struct rc5_data data = {0};

    data.command = val & ((1 << 6) - 1);
    val >>= 6;
    data.address = val & ((1 << 5) - 1);
    val >>= 5;
    data.control = val & 1;
    val >>= 1;
    unsigned field = val & 1;
    val >>= 1;

    if (!field) data.command += 64;
    return data;
}

int main() {
    usb_init();

    uint8_t pressed_keys[8] = {};
    gpio_init(25);
    gpio_set_dir(25, GPIO_OUT);
    gpio_put(25, 1);

    gpio_init(18);
    gpio_pull_up(18);
    gpio_set_dir(18, GPIO_IN);
    gpio_init(17);
    gpio_pull_up(17);
    gpio_set_dir(17, GPIO_IN);
    gpio_init(16);
    gpio_pull_up(16);
    gpio_set_dir(16, GPIO_IN);

    gpio_init(28);
    gpio_pull_up(28);
    gpio_set_dir(28, GPIO_IN);

    add_repeating_timer_us(IR_SAMPLE_US, irq_sample_ir, NULL, &timer_sample_ir);

    bool last_state = gpio_get(16);

    int i = 0;
    bool clear = false;

    struct rc5_data rc5_data = {0};
    uint32_t rc5_last = time_us_32();
    bool rc5_new = false;

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

        // Receive new IR keypress
        if (!ir.active) {
            bool data_new = false;
            struct rc5_data data;
            if (ir.value_bits == 13) {
                // New keypress has been received
                rc5_last = time_us_32();
                data = rc5_decode(ir.value);
                data_new = true;
            } else if (time_us_32() - rc5_last > 100000) {
                // Release the key if no signal has been received for a bit
                rc5_last = time_us_32();
                data = (struct rc5_data){0};
                data_new = true;
            }

            // Signal a new keystate if it differs
            if (data_new &&
                    memcmp(&rc5_data, &data, sizeof(struct rc5_data)) != 0) {
                rc5_new = true;
                rc5_data = data;
            }
            ir.value_bits = 0;
        }

        // Send new IR keypress
        if (rc5_new) {
            uint8_t key = 0;
            if (rc5_data.address == 0x18) {
                switch (rc5_data.command) {
                case RC5_A18_BUTTON_VOL_UP: key = HID_KEY_VOLUME_UP; break;
                case RC5_A18_BUTTON_VOL_DOWN: key = HID_KEY_VOLUME_DOWN; break;
                case RC5_A18_BUTTON_MUTE: key = HID_KEY_MUTE; break;

                case RC5_A18_BUTTON_UP: key = HID_KEY_ARROW_UP; break;
                case RC5_A18_BUTTON_DOWN: key = HID_KEY_ARROW_DOWN; break;
                case RC5_A18_BUTTON_LEFT: key = HID_KEY_ARROW_LEFT; break;
                case RC5_A18_BUTTON_RIGHT: key = HID_KEY_ARROW_RIGHT; break;
                case RC5_A18_BUTTON_OK: key = HID_KEY_ENTER; break;

                //case RC5_A18_BUTTON_HOME:
                //case RC5_A18_BUTTON_MENU:
                //case RC5_A18_BUTTON_INFO:
                //case RC5_A18_BUTTON_POWER:
                //case RC5_A18_BUTTON_PAG_DOWN:
                //case RC5_A18_BUTTON_BACK:
                //case RC5_A18_BUTTON_PAG_UP:
                //case RC5_A18_BUTTON_SEARCH:
                //case RC5_A18_BUTTON_STOP:
                }
            }

            // Release the key before making a new press
            if (key != 0) {
                keyboard_update(0, (uint8_t[6]){0});
                sleep_ms(1);
            }
            keyboard_update(0, (uint8_t[6]){key});
            rc5_new = false;
        }

        last_state = new_state;

        usb_task();
   }
}
