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
  size_t count = 0;
  for (; count < len; count++) {
    putch(*(char *)(buf + count));
  }
  return count;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
  if (ev.keycode == AM_KEY_NONE)
    return 0;
  char event[128];
  size_t count = 0;
  event[count++] = 'k';
  event[count++] = ev.keydown ? 'd' : 'u';
  event[count++] = ' ';
  const char *name = keyname[ev.keycode];
  while (*name != '\0') {
    event[count++] = *name++;
  }
  event[count++] = '\n';
  event[count++] = '\0';
  if (count > len) {
    count = len;
    event[count - 2] = '\n';
    event[count - 1] = '\0';
  }
  for (size_t i = 0; i < count; i++) {
    *(char *)(buf + i) = event[i];
  }
  return count;
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
  int width = io_read(AM_GPU_CONFIG).width;
  int height = io_read(AM_GPU_CONFIG).height;
  return sprintf(buf, "WIDTH:%d\nHEIGHT:%d", width, height);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  for (int i = 0; i < len; i++)
    io_write(AM_GPU_MEMCPY, offset, (void *)buf, len);
  io_write(AM_GPU_FBDRAW, 0, 0, NULL, 0, 0, true);
  return len;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
