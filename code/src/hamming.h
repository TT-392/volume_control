#pragma once

#include <stdio.h>
#include <stdint.h>

// PX P1 P2 10
// P3 09 08 07
// P4 06 05 04
// 03 02 01 00

#define P1(data) (((data) >> 9 ^ (data) >> 6 ^ (data) >> 2 ^ (data) >>10 ^ (data) >> 7 ^ (data) >> 4 ^ (data) >> 0) & 1)
#define P2(data) (((data) >>10 ^ (data) >> 8 ^ (data) >> 7 ^ (data) >> 5 ^ (data) >> 4 ^ (data) >> 1 ^ (data) >> 0) & 1)
#define P3(data) (((data) >> 9 ^ (data) >> 8 ^ (data) >> 7 ^ (data) >> 3 ^ (data) >> 2 ^ (data) >> 1 ^ (data) >> 0) & 1)
#define P4(data) (((data) >> 6 ^ (data) >> 5 ^ (data) >> 4 ^ (data) >> 3 ^ (data) >> 2 ^ (data) >> 1 ^ (data) >> 0) & 1)

#define PX(data) (((data) >> 0 ^ (data) >> 1 ^ (data) >> 2 ^ (data) >> 3 ^ (data) >> 4 ^ (data) >> 5 ^ (data) >> 6 ^\
                  (data) >> 7 ^ (data) >> 8 ^ (data) >> 9 ^ (data) >> 10 ^ P1(data) ^ P2(data) ^ P3(data) ^ P4(data)) & 1)


// PX P1 P2 10 P3 09 08 07 P4 06 05 04 03 02 01 00
// 15 14 13 12 11 10 09 08 07 06 05 04 03 02 01 00

#define hamming_16_11_encode(data) (\
        PX(data)           << 15 |\
        P1(data)           << 14 |\
        P2(data)           << 13 |\
        P3(data)           << 11 |\
        P4(data)           << 7  |\
        ((data)      & 0b0000000001111111) | \
        ((data << 1) & 0b0000011100000000) | \
        ((data << 2) & 0b0001000000000000) \
        )

uint16_t hamming_16_11_decode(uint16_t packet);

