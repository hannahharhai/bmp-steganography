#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>


int main(int argc, char *argv[]) {
    uint8_t a = 0xA1;
    uint8_t b = 0xB2;
    uint8_t c = 0xC3;

    unsigned bottom = a & 0x0F;
    unsigned top = a & 0xF0;
    // unsigned result = bottom << 4 | top >> 4;

    unsigned b_bottom = b & 0x0F;
    unsigned b_top = b & 0xF0;
    // unsigned b_result = b_bottom << 4 | b_top >> 4;

    unsigned c_bottom = c & 0x0F;
    unsigned c_top = c & 0xF0;
    unsigned c_result = c_bottom << 4 | c_top >> 4;

    unsigned final_result = (top << 4) | (b_top >> 4);
    // sizeof(final_result);

    printf("%x\n", final_result);

}