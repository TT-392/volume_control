// TODO: all the key stuff here should probably be called matrix stuff, leaving the key name for actual labeled keys
#pragma once
#include "defines.h"

void monitor_keys();
enum actions;

typedef struct key_event {
    enum actions action;
    int row;
    int col;
} key_event_t;

extern _Atomic bool key_matrix[5][8]; // TODO: remove


bool key_monitor_event_available();

key_event_t key_monitor_get_event();

