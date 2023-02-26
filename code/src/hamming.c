#include "usb.h"
#include <stdint.h>
#include <stdbool.h>
#define printf cdc_printf

// 15 14 13 12      PX P1 P2 10
// 11 10 09 08      P3 09 08 07
// 07 06 05 04      P4 06 05 04
// 03 02 01 00      03 02 01 00

#define P1_check(data) (((data) >> 14 ^ (data) >> 10 ^ (data) >>  6 ^ (data) >>  2 ^ (data) >> 12 ^ (data) >>  8 ^ (data) >>  4 ^ (data) >>  0) & 1)
#define P2_check(data) (((data) >> 13 ^ (data) >>  9 ^ (data) >>  5 ^ (data) >>  1 ^ (data) >> 12 ^ (data) >>  8 ^ (data) >>  4 ^ (data) >>  0) & 1)
#define P3_check(data) (((data) >> 11 ^ (data) >> 10 ^ (data) >>  9 ^ (data) >>  8 ^ (data) >>  3 ^ (data) >>  2 ^ (data) >>  1 ^ (data) >>  0) & 1)
#define P4_check(data) (((data) >>  7 ^ (data) >>  6 ^ (data) >>  5 ^ (data) >>  4 ^ (data) >>  3 ^ (data) >>  2 ^ (data) >>  1 ^ (data) >>  0) & 1)
#define PX_check(data) (((data) >>  0 ^ (data) >>  1 ^ (data) >>  2 ^ (data) >>  3 ^ (data) >>  4 ^ (data) >>  5 ^ (data) >>  6 ^ (data) >>  7 ^\
                         (data) >>  8 ^ (data) >>  9 ^ (data) >> 10 ^ (data) >> 11 ^ (data) >> 12 ^ (data) >> 13 ^ (data) >> 14 ^ (data >> 15)) & 1)

#define hamming_error_pos(data) ((~(\
        P1_check(data) << 0 |\
        P2_check(data) << 1 |\
        P3_check(data) << 2 |\
        P4_check(data) << 3 \
        ) & 0xf))

#define hamming_extract_data(data) (\
        ((data)      & 0b0000000001111111) | \
        ((data >> 1) & 0b0000001110000000) | \
        ((data >> 2) & 0b0000010000000000) \
        )
        


uint16_t hamming_16_11_decode(uint16_t packet) {
    int errpos = hamming_error_pos(packet);
    bool px_parity = PX_check(packet);

    if (errpos != 0xf) { // kinda got the bit positions reversed compared to other implementations, this means 0xf == no error
        if (px_parity == 1) {
            printf("HAMMING_DECODE: Recoverable error found at pos %i, in 0x%04x\r\n", errpos, packet);
            packet ^= 1 << errpos;
            printf("HAMMING_DECODE: Corrected packet: 0x%04x\r\n", packet);
        } else {
            printf("HAMMING_DECODE: Hamming error indicated but even extended parity");
            printf(" aborting, 0x%04x\r\n", packet);
            return 0;
        }
    } else if (px_parity == 1) {
        printf("HAMMING_DECODE: no error detected but overall parity failed, assuming flipped overal parity 0x%04x\r\n", packet);
    }

    return hamming_extract_data(packet);
}
