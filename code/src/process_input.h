#pragma once

enum actions {
    ACTION_NONE = 0,
    ACTION_UP = 0x8000,
    ACTION_DOWN = 0x4000
};

void process_input(enum actions action, uint8_t key);

