#include "pico/stdlib.h"
#include "defines.h"

static side_t defines;

void trrs_init() {
    defines = get_defines();

    uart_init(defines.uart, 115200);
    gpio_set_function(defines.uart_tx, GPIO_FUNC_UART);
    gpio_set_function(defines.uart_rx, GPIO_FUNC_UART);

    uart_set_fifo_enabled(defines.uart, true);
}

void trrs_send(uint16_t packet) {
    uart_putc_raw(defines.uart, packet & 0xff);
    uart_putc_raw(defines.uart, (packet >> 8) & 0xff);
}

uint16_t trrs_data_available() {
    return uart_is_readable(defines.uart);
}

uint16_t trrs_read() {
    uint16_t retval = uart_getc(defines.uart);

    if (!uart_is_readable_within_us(defines.uart, 1000))
        return 0x0000;

    retval |= uart_getc(defines.uart) << 8;
    return retval;
}
