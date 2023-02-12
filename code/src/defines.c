#include "defines.h"
#include "hamming.h"

#define ham_a hamming_16_11('a')
#define ham_b hamming_16_11('b')
#define ham_c hamming_16_11('c')
#define ham_d hamming_16_11('d')
#define ham_e hamming_16_11('e')
#define ham_f hamming_16_11('f')
#define ham_g hamming_16_11('g')
#define ham_h hamming_16_11('h')
#define ham_i hamming_16_11('i')
#define ham_j hamming_16_11('j')
#define ham_k hamming_16_11('k')
#define ham_l hamming_16_11('l')
#define ham_m hamming_16_11('m')
#define ham_n hamming_16_11('n')
#define ham_o hamming_16_11('o')
#define ham_p hamming_16_11('p')
#define ham_q hamming_16_11('q')
#define ham_r hamming_16_11('r')
#define ham_s hamming_16_11('s')
#define ham_t hamming_16_11('t')
#define ham_u hamming_16_11('u')
#define ham_v hamming_16_11('v')
#define ham_w hamming_16_11('w')
#define ham_x hamming_16_11('x')
#define ham_y hamming_16_11('y')
#define ham_z hamming_16_11('z')

static side_t left_side = {
    {11, 12, 13, 14, 15},
    5,
    {3, 4, 5, 6, 8, 9, 10},
    7,
    {{'#', '~', '1', '2', '3', '4', '5', ' '},
     {'#', 'T', 'q', 'w', 'e', 'r', 't', ' '},
     {'#', 'C', 'a', 's', 'd', 'f', 'g', ' '},
     {' ', 'S', 'z', 'x', 'c', 'v', 'b', ' '},
     {' ', 'C', 'S', 'A', '6', '_', '\n', ' '}},
    .led0 = 18,
    .led1 = 19,
    .uart_tx = 24,
    .uart_rx = 25,
    .uart = uart1
};

static side_t right_side = {
    {29, 27, 20, 16, 17},
    5,
    {21, 22, 24, 19, 25, 18, 26, 28},
    8,
    {{' ', '7', '8', '9', '0', '-', '+', 'B'},
     {'y', 'u', 'i', 'o', 'p', '[', ']', '\\'},
     {'h', 'j', 'k', 'l', ';', '\'', ' ', '\n'},
     {'n', 'm', ',', '.', '/', 'S', ' ', '#'},
     {'_', ' ', 'A', ' ', 'S', '#', ' ', '#'}},
    .led0 = 12,
    .led1 = 13,
    .uart_tx = 0,
    .uart_rx = 1,
    .uart = uart0
};


static side_t active_side;
static bool initialized = false;

side_t get_defines() {
    if (!initialized) {
        gpio_init(PIN_SIDE_DETECT);
        gpio_set_dir(PIN_SIDE_DETECT, false);

        if (gpio_get(PIN_SIDE_DETECT) == LEFT_SIDE) {
            active_side = left_side;
        } else {
            active_side = right_side;
        }
        initialized = true;
    }
    return active_side;
}

