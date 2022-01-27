#pragma once
#include <lib/stdint.h>
#include <lib/time.h>

void rtc_init();
void rtc_read();
time_t rtc_get_time();