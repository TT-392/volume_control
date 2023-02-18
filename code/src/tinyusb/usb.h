#pragma once


void usb_init();

void usb_task();

bool cdc_connected();

bool cdc_data_available();

void keyboard_update(uint8_t key_codes[6]);

static char* replace_lf_with_crlf_allocate_and_free(char* buffer);

void cdc_printf(const char *format, ...);



