//
// Created by alexander on 6/29/25.
//

#ifndef COMM_TEST_WIFI_H
#define COMM_TEST_WIFI_H

#include "lwipopts.h"
#include "time_util.h"
#include "pico/stdlib.h"
#include "pico/cyw43_arch.h"

#include "lwip/pbuf.h"
#include "lwip/udp.h"
#include "lwip/ip_addr.h"
#include "lwip/igmp.h"

#include "hardware/gpio.h"
#include "hardware/rtc.h"
#include "morse_code/morse_code.h"

#include <stdio.h>
#include <sys/time.h>
#include <sys/types.h>
#include <sys/select.h>
#include <unistd.h>

#ifndef WIFI_SSID
#define WIFI_SSID "NOT_SET"
#endif
#ifndef WIFI_PASSWORD
#define WIFI_PASSWORD "NOT_SET"
#endif

bool connect_wifi();

void print_network_status();

bool udp_multicast_init();

bool send_dht11_data(float humidity, float temperature);

bool scan_for_ssids();

#endif //COMM_TEST_WIFI_H
