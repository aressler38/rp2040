//
// Created by alexander on 6/30/24.
//


#include "comm_test.h"

static btstack_packet_callback_registration_t hci_event_callback_registration;
static int led_state = 0;

void blink_led(uint ms) {
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 1);
    sleep_ms(ms);
    cyw43_arch_gpio_put(CYW43_WL_GPIO_LED_PIN, 0);
    sleep_ms(ms);
}

void gpio_start() {
    gpio_init(GPIO_PIN_20);
    gpio_init(GPIO_PIN_21);
}

static const btstack_run_loop_t *the_run_loop = NULL;


void bt_comm_test_init() {}

float construct_float(uint8_t integral_part, uint8_t decimal_part) {
    float divisor = 1.0f;

    int decimal_places = 0;
    if (decimal_part < 10)
        decimal_places = 1;
    else if (decimal_part < 100)
        decimal_places = 2;

    // Calculate the divisor (10, 100, 1000, etc.)
    for (int i = 0; i < decimal_places; i++) {
        divisor *= 10.0f;
    }

    return (float) integral_part + (float) ((float) decimal_part / divisor);
}

int main(int argc, char *argv[]) {
    stdio_init_all();
    // setup_default_uart();
    if (cyw43_arch_init()) {
        printf("Failed to init Wi-Fi\n\n");
        return 1;
    }

    // Enable station (client) mode
    cyw43_arch_enable_sta_mode();
    while (!connect_wifi()) {
        printf("Failed to connect to WiFi\n\n");
        sleep_ms(1000);
    }

    float sys_clk_freq = 0;
    get_sys_clk_freq(&sys_clk_freq);

    datetime_t datetime;
    init_system_hardware_rtc(&datetime);

    const static unsigned int BUFFER_LENGTH = 32;
    char buffer[BUFFER_LENGTH];

    fd_set rdfs;
    //struct timeval tv = {.tv_sec = 1, .tv_usec = 0};
    //int retval;
    //int read_count = 0;
    //char *words[1] = {buffer};
    FD_ZERO(&rdfs);
    FD_SET(0, &rdfs);
    PIO pio = pio0;
    uint offset;
    uint8_t dht11_data_buffer[5];

    bool success = false;

    /* \33[ is the escape sequence starter.  2J - clear screen */
    printf("\33[2J\33[32;40;1m");
    printf("------ONLINE--------\n");
    printf("The system clock frequency is: %f\n", sys_clk_freq);
    printf("System clock: %lu Hz\n", clock_get_hz(clk_sys));

    print_network_status();

    //gpio_init(DHT11_PIN);
    //gpio_set_dir(DHT11_PIN, GPIO_OUT);

    //uint16_t clock_divisor = 65535;
    //success = setup_pio(&dht11_read_program, &pio, &sm, &offset, DHT11_PIN, clock_divisor);
    //        hard_assert(success);

    //pio_sm_set_enabled(pio, sm, false);

    // Disable program
    //pio_set_sm_mask_enabled(pio, (1 << sm), false);
    uint sm = pio_claim_unused_sm(pio, true);
    if (sm == -1) {
        printf("Failed to claim state machine\n");
        return 1;
    }
    dht11_setup_pio(pio, sm, DHT11_PIN);
    dht11_setup_dma(pio, sm, true);

    if (!udp_multicast_init()) {
        printf("Failed to initialize UDP multicast\n");
        return 1;
    }
    printf("PIO claimed: %d\n", pio_sm_is_claimed(pio, sm));
    printf("FIFO level: %d\n", pio_sm_get_rx_fifo_level(pio, sm));

    while (true) {
        blink_led(400);
        // printf("\33[2J");

        printf("--------------\n");
        print_time(&datetime, buffer, BUFFER_LENGTH);
        //scan_for_ssids();

        //dht11_send_start(DHT11_PIN); // CPU-driven reset pulse
        dht11_read(pio, sm, dht11_data_buffer);

        float humidity = construct_float(dht11_data_buffer[0], dht11_data_buffer[1]);
        float temperature = construct_float(dht11_data_buffer[2], dht11_data_buffer[3]);

        printf("Humidity: %.2f%%, Temperature: %.2f C\n", humidity, temperature);
        printf("Humidity: %d.%d%%, Temperature: %d.%d C\n", dht11_data_buffer[0], dht11_data_buffer[1],
               dht11_data_buffer[2], dht11_data_buffer[3]);

// TODO - example https://github.com/raspberrypi/pico-examples/blob/master/pio/squarewave/squarewave_div_sync.c

        send_dht11_data(humidity, temperature);
        memset(buffer, 0, BUFFER_LENGTH);
        // return 0;
    }
    return 0;
}