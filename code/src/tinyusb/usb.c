#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <stdarg.h>
#include "tusb.h"
#include "tinyusb/usb_descriptors.h"


void usb_init() {
    tusb_init();
}

void usb_task() {
    tud_task();
}

bool cdc_connected() {
    return tud_cdc_connected();
}

bool cdc_data_available() {
    return tud_cdc_available();
}

void keyboard_update(uint8_t key_codes[6]) {
    // Remote wakeup
    if (tud_suspended()) {
        // Wake up host if we are in suspend mode
        // and REMOTE_WAKEUP feature is enabled by host
        tud_remote_wakeup();
    }


    if (tud_hid_ready()) {
        tud_hid_keyboard_report(ITF_NUM_KEYBOARD, 0, key_codes);
    }

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


