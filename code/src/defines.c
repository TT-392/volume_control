#include "defines.h"

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
    0,
    0
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
    0,
    0
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

