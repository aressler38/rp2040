//
// Created by alexander on 6/24/25.
//

#ifndef COMM_TEST_DHT11_H
#define COMM_TEST_DHT11_H

#include "hardware/pio.h"
#include "hardware/pio_instructions.h"
#include "hardware/dma.h"
#include "pio.h"
#include <math.h>

#define DHT11_BUFFER_SIZE_BITS 40
#define DHT11_PIN 16
// Note: You'll need to define DHT11_THRESHOLD appropriately
// (e.g., ~50–60 cycles for 0 vs 1 depending on clock divider).
#define DHT11_THRESHOLD 50
// 1e6 is 1 MHz
//#define DHT11_SAMPLE_FREQUENCY 1e6
#define DHT11_SAMPLE_FREQUENCY 1e6


#define BYTE_TO_BINARY_PATTERN "%c%c%c%c%c%c%c%c"
#define BYTE_TO_BINARY(byte)  \
  ((byte) & 0x80 ? '1' : '0'), \
  ((byte) & 0x40 ? '1' : '0'), \
  ((byte) & 0x20 ? '1' : '0'), \
  ((byte) & 0x10 ? '1' : '0'), \
  ((byte) & 0x08 ? '1' : '0'), \
  ((byte) & 0x04 ? '1' : '0'), \
  ((byte) & 0x02 ? '1' : '0'), \
  ((byte) & 0x01 ? '1' : '0')

void get_sys_clk_freq(float *freq);

bool dht11_read(PIO pio, uint sm, uint8_t data[5]);

void dht11_setup_dma(PIO pio, uint sm, bool irq0_enabled);

void dht11_setup_pio(PIO pio, uint sm, uint pin);

void dht11_send_start(uint pin);

#endif //COMM_TEST_DHT11_H
