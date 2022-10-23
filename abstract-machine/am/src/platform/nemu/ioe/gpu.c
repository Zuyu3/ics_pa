#include <am.h>
#include <nemu.h>
#include<stdio.h>


#define SYNC_ADDR (VGACTL_ADDR + 4)
#define GPU_W inw(VGACTL_ADDR + 2)
#define GPU_H inw(VGACTL_ADDR)


void __am_gpu_init() {
  int i;
  int w = GPU_W;  // TODO: get the correct width
  int h = GPU_H;  // TODO: get the correct height
  printf("%d, %d", w, h);
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = 256;
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = GPU_W, .height = GPU_H,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {

  for(int i = ctl->y; i < ctl->y + ctl->h; i++)
    for(int j = ctl->x; j< ctl->x + ctl->w; j++)
       outl(FB_ADDR + i * GPU_W + j, (uintptr_t)ctl->pixels + (i + j- ctl->x - ctl->y));
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
