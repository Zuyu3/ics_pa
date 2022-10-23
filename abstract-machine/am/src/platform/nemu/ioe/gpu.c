#include <am.h>
#include <nemu.h>
#include<stdio.h>


#define SYNC_ADDR (VGACTL_ADDR + 4)
#define GPU_W io_read(AM_GPU_CONFIG).width
#define GPU_H inw(VGACTL_ADDR)


void __am_gpu_init() {
  int i;
  int w = GPU_W;  // TODO: get the correct width
  int h = GPU_H;  // TODO: get the correct height
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) {fb[i] = i * 256 / w * h; fb[i] |= fb[i] << 16 | fb[i] << 8;}
  outl(SYNC_ADDR, 1);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = GPU_W, .height = GPU_H,
    .vmemsz = GPU_W * GPU_H
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  uint32_t *sr = (uint32_t *)(uintptr_t)ctl->pixels;
  for(int i = 0; i < ctl->h; i++)
    for(int j = 0; j< ctl->w; j++)
       fb[(ctl->y + i) * GPU_W + ctl->x + j] = sr[i * ctl->w + j];
  
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
