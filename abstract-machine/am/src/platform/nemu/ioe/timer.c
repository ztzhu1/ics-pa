#include <am.h>
#include <nemu.h>
#include <stdio.h>

static AM_TIMER_UPTIME_T boot_time = {.us = 0};
static bool boot = false;

static void get_time(AM_TIMER_UPTIME_T *t) {
  t->us = *(volatile uint64_t *)RTC_ADDR;
}

void __am_timer_init() {
  boot = true;
  get_time(&boot_time);
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  if (!boot) {
    boot = true;
    __am_timer_init();
  }
  AM_TIMER_UPTIME_T now;
  get_time(&now);
  uptime->us = now.us - boot_time.us;
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
