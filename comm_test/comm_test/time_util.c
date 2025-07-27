//
// Created by alexander on 6/29/25.
//
#include "time_util.h"

void format_datetime(datetime_t *datetime, char *buffer, unsigned int len) {
    sprintf(buffer, "%d-%02d-%02d %02d-%02d-%02d",
            datetime->year, datetime->month, datetime->day,
            datetime->hour, datetime->min, datetime->sec);
}

void print_time(datetime_t *datetime, char *buffer, unsigned int len) {
    rtc_get_datetime(datetime);
    datetime_to_str(buffer, len, datetime);
    format_datetime(datetime, buffer, len);
    printf("The time is: %s\n", buffer);
}

/**
 * This should be done once.
 */
void init_system_hardware_rtc(datetime_t *datetime) {
    datetime->year = 2024;
    datetime->month = 07;
    datetime->day = 01;
    datetime->dotw = 1;
    datetime->hour = 0;
    datetime->min = 0;
    datetime->sec = 0;
    rtc_init();
    rtc_set_datetime(datetime);
    // clk_sys is >2000x faster than clk_rtc, so datetime is not updated immediately when rtc_get_datetime() is called.
    // tbe delay is up to 3 RTC clock cycles (which is 64us with the default clock settings)
    sleep_us(64);
}
