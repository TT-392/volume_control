#include <stdint.h>

void matrix_init();

void matrix_clear();

void matrix_set_pixel(int x, int y, uint8_t brightness);

void matrix_fill_rect(int x1, int y1, int x2, int y2, uint8_t brightness);

void matrix_update();

void matrix_put(char display[10][72]);
