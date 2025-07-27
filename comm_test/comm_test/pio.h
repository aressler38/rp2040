//
// Created by alexander on 6/29/25.
//

#ifndef COMM_TEST_PIO_H
#define COMM_TEST_PIO_H

#include "hardware/pio.h"
#include "hardware/pio_instructions.h"
#include "dht11.pio.h"
#include "dht11.h"

bool setup_pio(const pio_program_t *program, PIO *pio, uint *sm, uint *offset, uint pin, uint16_t divisor);

#endif //COMM_TEST_PIO_H
