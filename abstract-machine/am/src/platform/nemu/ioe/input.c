#include <am.h>
#include <nemu.h>
#include <stdio.h>

#define KEYDOWN_MASK 0x8000

static bool initKbd = false;

void __am_input_keybrd(AM_INPUT_KEYBRD_T *kbd) {
  if (!initKbd) {
    kbd->keydown = 0;
    kbd->keycode = AM_KEY_NONE;
    initKbd = true;
  }

  uint32_t k = *(uint32_t *)KBD_ADDR;
  kbd->keydown = (k & KEYDOWN_MASK ? true : false);
  kbd->keycode = k & ~KEYDOWN_MASK;
  if (kbd->keycode != 0)
    printf("%d\n", kbd->keycode);
}
