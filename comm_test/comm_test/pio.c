//
// Created by alexander on 6/29/25.
//
#include "pio.h"

bool setup_pio(const pio_program_t *program, PIO *pio, uint *sm, uint *offset, uint pin, uint16_t divisor) {

    // load the program on a free pio and state machine
    // Note: This uses the LAST free pio first to reduce the chance of causing problems for old code
    // that assumes the first pio is free
    if (!pio_claim_free_sm_and_add_program(program, pio, sm, offset)) {
        return false;
    }

    // Set this pin's GPIO function (connect PIO to the pad)
    pio_gpio_init(*pio, pin);

    // Set the pin direction to output with the PIO
    pio_sm_set_consecutive_pindirs(*pio, *sm, pin, 1, true);

    pio_sm_config c = dht11_read_program_get_default_config(*offset);
    sm_config_set_set_pins(&c, pin, 1);

    // Load our configuration, and jump to the start of the program
    pio_sm_init(*pio, *sm, *offset, &c);

    // set the pio divisor
    pio_sm_set_clkdiv(*pio, *sm, divisor);
    return true;
}
