#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define SYNC_ADDR (VGACTL_ADDR + 4)
#define WIDTH 400
#define HEIGHT 300
#define N 20

void __am_gpu_init() {
  // int i;
  // int w = WIDTH;
  // int h = HEIGHT;
  // uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
  // for (i = 0; i < w * h; i ++) fb[i] = i;
  // *(uint32_t *)(VGACTL_ADDR) = 0x80000000 | *(uint32_t *)(VGACTL_ADDR);
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
    // outl(SYNC_ADDR, 1);
    *(uint32_t *)(VGACTL_ADDR) = 0x80000000 | *(uint32_t *)(VGACTL_ADDR);
  }
  int x = ctl->x, y = ctl->y, w = ctl->w, h = ctl->h;
  for (int i = y; i < y + h; i++) {
    for (int j = x; j < x + w; j++) {
      int index = j + i * WIDTH;
      uint32_t *fb = (uint32_t *)(uintptr_t)FB_ADDR;
      fb[index] = ((uint32_t *)ctl->pixels)[(j - x) + (i - y) * N];
    }
  }
}

void __am_gpu_status(AM_GPU_STATUS_T *status) {
  status->ready = true;
}
