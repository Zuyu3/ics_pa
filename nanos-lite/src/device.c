#include <common.h>

#if defined(MULTIPROGRAM) && !defined(TIME_SHARING)
# define MULTIPROGRAM_YIELD() yield()
#else
# define MULTIPROGRAM_YIELD()
#endif

#define NAME(key) \
  [AM_KEY_##key] = #key,

static const char *keyname[256] __attribute__((used)) = {
  [AM_KEY_NONE] = "NONE",
  AM_KEYS(NAME)
};


size_t serial_write(const void *buf, size_t offset, size_t len) {
  //ignore offset.
    for(int i = 0; i < len; i++)
      putch(((char *)buf)[i]);
    return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  int res = 0;
  
  AM_INPUT_KEYBRD_T kbd = io_read(AM_INPUT_KEYBRD);

  if(kbd.keycode == AM_KEY_NONE)
    return 0;

  if(kbd.keydown)
    strcpy(buf, "kd ");
  else
    strcpy(buf, "ku ");

  strcpy(buf + 3, keyname[kbd.keycode]);

  res = strlen(buf);
  if(res > len) {
    ((char *)buf)[0] = '\0';
    return 0;
  }
  return res;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T gpu_info = io_read(AM_GPU_CONFIG);

  int res;

  res = sprintf(buf, "WIDTH:%d\nHEIGHT:%d\n", gpu_info.width, gpu_info.height);
  if(res > len)
    panic("Error. Buf too small to read dispinfo\n");
  
  return res;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  AM_GPU_CONFIG_T gpu_info = io_read(AM_GPU_CONFIG);
  int screen_w = gpu_info.width;
  //int screen_h = gpu_info.height;

  int x, y;
  //int width, height;
  x = (offset / 4) % screen_w;
  y = (offset / 4) / screen_w;
  printf("os write: (%d, %d), len=%d\n\n", x, y, len);
  

  //TODO:Maybe bugs here.
  io_write(AM_GPU_FBDRAW, x, y, (void *)buf, 1, len / 4, true);
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
