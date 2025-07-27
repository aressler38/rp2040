//
// Created by alexander on 6/24/25.
//

#include <pico/time.h>
#include <hardware/clocks.h>
#include <pico/printf.h>
#include <string.h>
#include "dht11.pio.h"
#include "dht11.h"

// DHT11 result structure
typedef struct dht11_read_result {
    float humidity;
    float temperature;
} dht11_read_result;


void dht11_send_start(uint pin) {
    gpio_set_dir(pin, GPIO_OUT);
    gpio_put(pin, 0);
    sleep_ms(20); // ≥18 ms low
    gpio_put(pin, 1);
    sleep_us(40); // Wait before switching to input
    gpio_set_dir(pin, GPIO_IN);
}


void get_sys_clk_freq(float *freq) {
    *freq = (float) clock_get_hz(clk_sys);
}

/**
 * state_machine_index
 * target_freq (Hz) can be 2e6 or whatever is required for the  PIO clock
 */
void set_pio_clock_div(PIO pio, uint state_machine_index, float target_freq) {
    float clk_div = (float) clock_get_hz(clk_sys) / target_freq;
    pio_sm_set_clkdiv(pio, state_machine_index, clk_div);
}

#define DMA_BUFFER_SIZE 40
static uint32_t dma_buffer[DMA_BUFFER_SIZE];
static const uint32_t max_counter = 255;
static const uint32_t pulse_threshold = 20;

int dma_channel_global = 0;

void print_dma_buffer() {
    printf("---- DMA Buffer ----\n");
    int j = 0;
    for (int i = 0; i < DMA_BUFFER_SIZE; ++i, ++j) {
        if (j % 8 == 0) printf("\n");

        printf("%d ", max_counter - dma_buffer[i] > 20 ? 1 : 0);
    }
    printf("\n");
}

void dma_irq0_callback() {
    // Check which DMA channel triggered the IRQ
    uint32_t ints = dma_hw->ints0;

    // Clear the interrupt
    //dma_hw->ints0 = ints;
    dma_channel_acknowledge_irq0(dma_channel_global);
    printf("irq0 triggered\n");


    //printf("-------->DMA IRQ 0 %s %lu %lu %lu %lu\n",
    //       max_counter - dma_buffer[0] > 20 ? "long" : "short",
    //       dma_buffer[0], dma_buffer[1], dma_buffer[2], dma_buffer[3]);

    // Restart the DMA for the next transfer
    dma_channel_config c = dma_channel_get_default_config(dma_channel_global);
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);

    channel_config_set_dreq(&c, pio_get_dreq(pio0, 0, false));

    dma_channel_set_read_addr(dma_channel_global, &pio0->rxf[0], false); // Don't trigger yet
    dma_channel_set_write_addr(dma_channel_global, dma_buffer, false);
    dma_channel_set_trans_count(dma_channel_global, DMA_BUFFER_SIZE, true); // Trigger restart
}

void dht11_setup_dma(PIO pio, uint sm, bool irq0_enabled) {
    dma_channel_global = dma_claim_unused_channel(true);
    dma_channel_config c = dma_channel_get_default_config(dma_channel_global);

    // Configure DMA to read from PIO RX FIFO
    channel_config_set_transfer_data_size(&c, DMA_SIZE_32);
    channel_config_set_read_increment(&c, false);
    channel_config_set_write_increment(&c, true);
    //channel_config_set_ring(&c, true, 8);

    memset(dma_buffer, 0, DMA_BUFFER_SIZE * sizeof(uint32_t));

    // Set up the DMA request for PIO RX FIFO
    channel_config_set_dreq(&c, pio_get_dreq(pio, sm, false));
    dma_channel_configure(dma_channel_global, &c, dma_buffer, &pio->rxf[sm], DMA_BUFFER_SIZE,
                          false);

    printf("IRQ Enabled: %s\n", irq0_enabled ? "true" : "false");
    if (irq0_enabled) {
        dma_channel_set_irq0_enabled(dma_channel_global, true);
        // Configure the processor to run dma_handler() when DMA IRQ 0 is asserted
        irq_set_exclusive_handler(DMA_IRQ_0, dma_irq0_callback);
        irq_set_enabled(DMA_IRQ_0, true);
    }
    dma_channel_start(dma_channel_global);
}

void dht11_setup_pio(PIO pio, uint sm, uint pin) {
    pio_sm_set_enabled(pio, sm, false);
    pio_sm_clear_fifos(pio, sm);

    uint offset = pio_add_program(pio, &dht11_read_program);
    printf("Program loaded at offset %d\n", offset);

    pio_sm_config c = dht11_read_program_get_default_config(offset);
    //sm_config_set_fifo_join(&c, PIO_FIFO_JOIN_TX );

    float sys_clk = (float) clock_get_hz(clk_sys);
    float clkdiv = sys_clk / DHT11_SAMPLE_FREQUENCY;
    printf("System clock: %.0f Hz\n", sys_clk);
    printf("Target frequency: %.0f Hz\n", DHT11_SAMPLE_FREQUENCY);
    printf("Using clock divider: %.3f\n", clkdiv);
    printf("Target frequency: %.0f Hz\n", sys_clk / clkdiv / 2); // Divide by 2 instructions

    sm_config_set_set_pins(&c, pin, 1);
    pio_gpio_init(pio, pin);
    pio_sm_set_consecutive_pindirs(pio, sm, pin, 1, true); // output

    sm_config_set_clkdiv(&c, clkdiv);

    sm_config_set_in_pins(&c, pin);
    sm_config_set_out_pins(&c, pin, 1);
    sm_config_set_jmp_pin(&c, pin);
    pio_sm_init(pio, sm, offset, &c);
    pio_sm_set_enabled(pio, sm, true);

    // Check if it's running using the CTRL register
    uint enabled = pio->ctrl & (1u << sm);
    printf("State machine running: %d\n", enabled ? 1 : 0);

    // You can use pio_sm_set_clkdiv to set the frequence AFTER the sm is started.
    //pio_sm_set_clkdiv(*pio, sm, clkdiv);
}

// TODO - example https://github.com/raspberrypi/pico-examples/blob/master/pio/squarewave/squarewave_div_sync.c

bool dht11_read(PIO pio, uint sm, uint8_t data[5]) {
    //dht11_send_start(DHT11_PIN); // CPU-driven reset pulse

    pio_sm_exec(pio, sm, pio_encode_jmp(0));

    uint32_t wait_for_40_ms = (uint32_t) roundf(
            0.020f / (1.0f / DHT11_SAMPLE_FREQUENCY)); // when it is 1MHz it should equal 40000
    // I discovered that jmp x-- {label} only costs 1 cycle.

    pio_sm_put_blocking(pio, sm, wait_for_40_ms);
    pio_sm_put_blocking(pio, sm, max_counter);

    sleep_ms(500);
    pio_sm_exec(pio, sm, pio_encode_jmp(0));

    //do {
    //    uint32_t word = pio_sm_get_blocking(pio, sm);
    //    printf("%lu ", word);
    //} while (0 != pio_sm_get_rx_fifo_level(pio, sm));
    printf("Resetting DMA\n");

    print_dma_buffer();


    memset(data, 0, sizeof(uint8_t) * 5);

    uint32_t counter = max_counter;
    for (int i = 0; i < 5; ++i) {
        data[i] = 0;

        for (int j = 7; 0 < j; --j) {
            int value = counter - dma_buffer[8 * i + j] > pulse_threshold ? 1 : 0;
            data[i] |= value << (7 - j);
            //counter = dma_buffer[8 * i + j];
        }
    }

    memset(dma_buffer, 0, sizeof(uint32_t) * DMA_BUFFER_SIZE);

    // Verify checksum
    uint8_t checksum = data[0] + data[1] + data[2] + data[3];
    return checksum == data[4];
}