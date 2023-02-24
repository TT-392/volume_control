#pragma once
#include "defines.h"

void monitor_keys();
enum actions;

typedef struct matrix_event {
    enum actions action;
    int row;
    int col;
} matrix_event_t;


bool matrix_monitor_event_available();

matrix_event_t matrix_monitor_get_event();

