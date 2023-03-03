#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "tusb.h"
#include "tinyusb/usb_descriptors.h"
#include "pico/bootrom.h"
#include "usb.h"

#define printf cdc_printf

static void handle_cdc_input();

void usb_init() {
    tusb_init();
}

void usb_task() {
    tud_task();

    if (tud_connected()) {
        if (tud_cdc_available()) {
            tud_cdc_write_flush();
            handle_cdc_input();
        }
    }
}

bool usb_connected() {
    return tud_connected();
}

#define INPUT_MAX_LEN 100 // actual data is 101 because of the \0
static char received_data[INPUT_MAX_LEN + 1] = "";

bool cdc_data_available() {
    return received_data[0] != '\0';
}

void cdc_get_line(char buffer[INPUT_MAX_LEN + 1]) { // INPUT_MAX_LEN + 1 is 101
    memcpy(buffer, received_data, INPUT_MAX_LEN + 1);
    received_data[0] = '\0';
}

static void handle_cdc_input() {
    uint8_t inbuf[64];
    uint32_t inlen = tud_cdc_n_read(ITF_NUM_CDC, inbuf, sizeof(inbuf));

    static char buffer[INPUT_MAX_LEN + 1] = "";
    static int ind = 0;

    for (int i = 0; i < inlen; i++) {
        char c = inbuf[i];
        if (c == '\n') {
            buffer[ind++] = '\0';

            if (strcmp(buffer, "reset") == 0) {
                reset_usb_boot(0, 0);
            }

            strcpy(received_data, buffer);
            ind = 0;
        } else if (c != '\r') {
            if (ind < INPUT_MAX_LEN)
                buffer[ind++] = c;
        }
    }
}

void keyboard_update(uint8_t modifiers, uint8_t key_codes[6]) {
    // Remote wakeup
    if (tud_suspended()) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }


    while (!tud_hid_ready()) { //TODO: a timeout might be desirable here
        sleep_ms(1);
        tud_task();
    }

    tud_hid_keyboard_report(0, modifiers, key_codes);
}

static char* replace_lf_with_crlf_allocate_and_free(char* buffer) {
    // OPTIMIZATION: using strtok and strcpy could theoretically maybe make things slightly faster, but I was being lazy, not a very high priority
   
    int count = 0;
    for (int i = 0; i < strlen(buffer); i++) {
        if (buffer[i] == '\n') {
            count++;
        }
    }

    char* retval = malloc(count + strlen(buffer) + 1); // UNSAFE: check if malloc failed
    int j = 0;

    for (int i = 0; i < strlen(buffer) + 1; i++) {
        if (buffer[i] == '\n') {
            retval[j++] = '\r';
        } 
        retval[j++] = buffer[i];
    }

    free(buffer);
    return retval;
}

void cdc_printf(const char *format, ...) {
    va_list args;
    va_start(args, format);
    size_t len = vsnprintf(NULL, 0, format, args);

    char *buffer = malloc(len + 1);

    va_start(args, format);
    vsprintf(buffer, format, args);
    va_end(args);

    buffer = replace_lf_with_crlf_allocate_and_free(buffer);

    tud_cdc_write_str(buffer);
    tud_cdc_write_flush();

    free(buffer);
}



// Invoked when sent REPORT successfully to host
void tud_hid_report_complete_cb(uint8_t instance, uint8_t const *report, uint16_t len) {
}

// Invoked when received GET_REPORT control request
uint16_t tud_hid_get_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t *buffer, uint16_t reqlen) {
    return 0;
}

// Invoked when received SET_REPORT control request or
void tud_hid_set_report_cb(uint8_t instance, uint8_t report_id, hid_report_type_t report_type, uint8_t const *buffer, uint16_t bufsize) {
}


