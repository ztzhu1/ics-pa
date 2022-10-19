#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
#define WIDTH 400
#define HEIGHT 300

void __am_gpu_init() {
  int i;
  int w = WIDTH;
  int h = HEIGHT;
  uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  for (i = 0; i < w * h; i ++) fb[i] = i;
  // outl(VGACTL_ADDR, WIDTH << 16 | HEIGHT);
  // outl(SYNC_ADDR, 0);
}

void __am_gpu_config(AM_GPU_CONFIG_T *cfg) {
  *cfg = (AM_GPU_CONFIG_T) {
    .present = true, .has_accel = false,
    .width = WIDTH, .height = HEIGHT,
    .vmemsz = 0
  };
}

void __am_gpu_fbdraw(AM_GPU_FBDRAW_T *ctl) {
  if (ctl->sync) {
    outl(SYNC_ADDR, 1);
  }
  __am_gpu_init();
  // int x = ctl->x, y = ctl->y, w = ctl->w;
  // uint32_t index = x + y * w;
  // uint32_t offset = index * 32;
  // outl(FB_ADDR + offset, *(uint32_t *)(ctl->pixels + index));
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
