#include <am.h>
#include <sys/time.h>
#include <unistd.h>
#include <NDL.h>
#include <assert.h>
#include <string.h>

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};


void __am_timer_init() {
}
void __am_gpu_init() {
}

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  char buf[100];

  if(NDL_PollEvent(buf, 100)) {

    kbd->keydown = strncmp(buf, "ku", 2)? true: false;
    for(int i = 0; i < 256; i++) {
      if(!strcmp(keyname[i], buf+3)) {
        printf("ioe get: %d\n", i);
        kbd->keycode = i;
        break;
      }
    }
  }
  else {
    kbd->keycode = AM_KEY_NONE;
    kbd->keydown = false;
  }
}

void __am_timer_rtc(AM_TIMER_RTC_T *rtc) {
  rtc->second = 0;
  rtc->minute = 0;
  rtc->hour   = 0;
  rtc->day    = 0;
  rtc->month  = 0;
  rtc->year   = 1900;
}

void __am_timer_uptime(AM_TIMER_UPTIME_T *uptime) {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  uptime->us = tv.tv_usec + tv.tv_sec * 1000000;
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  int w = 0, h = 0;
  NDL_OpenCanvas(&w, &h);
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = w, .height = h,
    .vmemsz = 0
  };
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  NDL_DrawRect(ctl->pixels, ctl->x, ctl->y, ctl->w, ctl->h);
}

static void __am_timer_config(AM_TIMER_CONFIG_T *cfg) { cfg->present = true; cfg->has_rtc = true; }
static void __am_input_config(AM_INPUT_CONFIG_T *cfg) { cfg->present = true;  }
static void __am_uart_config(AM_UART_CONFIG_T *cfg)   { cfg->present = false; }
static void __am_net_config (AM_NET_CONFIG_T *cfg)    { cfg->present = false; }

typedef void (*handler_t)(void *buf);
static void *lut[128] = {
  [AM_TIMER_CONFIG] = __am_timer_config,
  [AM_TIMER_RTC   ] = __am_timer_rtc,
  [AM_TIMER_UPTIME] = __am_timer_uptime,
  [AM_INPUT_CONFIG] = __am_input_config,
  [AM_INPUT_KEYBRD] = __am_input_keybrd,
  [AM_GPU_CONFIG  ] = __am_gpu_config,
  [AM_GPU_FBDRAW  ] = __am_gpu_fbdraw,
  [AM_GPU_STATUS  ] = __am_gpu_status,
  [AM_UART_CONFIG ] = __am_uart_config,
  [AM_NET_CONFIG  ] = __am_net_config,
};

static void fail(void *buf) { assert(0); }

bool ioe_init() {
  for (int i = 0; i < sizeof(lut) / sizeof(lut[0]); i++)
    if (!lut[i]) lut[i] = fail;
  __am_gpu_init();
  __am_timer_init();
  return true;
}

void ioe_read (int reg, void *buf) {/*printf("ioe read: %d\n", reg);*/ ((handler_t)lut[reg])(buf); }
void ioe_write(int reg, void *buf) {/*printf("ioe write: %d\n", reg);*/ ((handler_t)lut[reg])(buf); }
