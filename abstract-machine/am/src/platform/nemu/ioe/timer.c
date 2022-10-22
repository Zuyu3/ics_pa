#include <am.h>
#include <nemu.h>
#include <stdio.h>

void __am_timer_init() {
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  printf("%d  ", *(int *)(RTC_ADDR));
  printf("%d   \n",   inl(RTC_ADDR));
  //ioe_write(uptime->us, (void *)RTC_ADDR);
  //printf("%d  \n", uptime->us);
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}
