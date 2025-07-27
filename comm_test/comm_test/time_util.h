//
// Created by alexander on 6/29/25.
//

#ifndef COMM_TEST_TIME_H
#define COMM_TEST_TIME_H

#include "hardware/rtc.h"
#include <hardware/clocks.h>
#include "pico/stdlib.h"
#include "pico/util/datetime.h"
#include <stdio.h>

void format_datetime(datetime_t *datetime, char *buffer, unsigned int len);

void print_time(datetime_t *datetime, char *buffer, unsigned int len);

void init_system_hardware_rtc(datetime_t *datetime);

#endif //COMM_TEST_TIME_H
