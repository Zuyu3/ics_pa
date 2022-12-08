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

  res = snprintf(buf, len, "WIDTH:%d\nHEIGHT:%d", gpu_info.width, gpu_info.height);
  printf("%s\n", buf);
  if(res > len)
    panic("Error. Buf too small to read dispinfo\n");
  
  return res;
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
