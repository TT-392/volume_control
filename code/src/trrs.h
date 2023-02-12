#pragma once

void trrs_init();

void trrs_send(uint16_t packet);

bool trrs_data_ready();

uint16_t trrs_read();

