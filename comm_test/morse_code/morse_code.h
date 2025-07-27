//
// Created by alexander on 7/5/24.
//

#ifndef COMM_TEST_MORSE_CODE_H
#define COMM_TEST_MORSE_CODE_H

#define MORSE_CODE_TIME_DIT 1
#define MORSE_CODE_TIME_DAH 3
#define MORSE_CODE_TIME_INTRA_CHAR 1
#define MORSE_CODE_TIME_INTER_CHAR 3
#define MORSE_CODE_TIME_WORD 7

#define MORSE_CODE_DIT 0x1
#define MORSE_CODE_DAH 0x2

#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"
#include <ctype.h>

void morse_code_flash(char *words[], size_t count);

#endif //COMM_TEST_MORSE_CODE_H
