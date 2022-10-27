#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <assert.h>

static int evtdev = -1;
static int fbdev = -1;
static int screen_w = 0, screen_h = 0;
static int canvas_w = 0, canvas_h = 0;
static int canvas_x = 0, canvas_y = 0;

extern int _gettimeofday(struct timeval *tv, struct timezone *tz);
extern int _open(const char *path, int flags, mode_t mode);
extern int _read(int fd, void *buf, size_t count);
extern int _write(int fd, void *buf, size_t count);
extern off_t _lseek(int fd, off_t offset, int whence);
extern int _close(int fd);

uint32_t NDL_GetTicks() {
  struct timeval tv;
  _gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000 + tv.tv_usec;
}

int NDL_PollEvent(char *buf, int len) {
  int fd = _open("/dev/events", 0, 0);
  int ret = _read(fd, buf, len);
  _close(fd);
  if (ret != 0) { ret = 1; }
  return ret;
}

void NDL_OpenCanvas(int *w, int *h) {
  if (*w == 0)
    *w = screen_w;
  if (*h == 0)
    *h = screen_h;
  assert(*w <= screen_w && *h <= screen_h);
  // canvas size
  canvas_w = *w;
  canvas_h = *h;
  printf("\033[36mcanvas width: %d, canvas height: %d\033[0m\n", canvas_w, canvas_h);

  // canvas position 
  // canvas_x = 136;
  // canvas_y = 86;
  printf("\033[36mcanvas x: %d, canvas y: %d\033[0m\n", canvas_x, canvas_y);

  if (getenv("NWM_APP")) {
    int fbctl = 4;
    fbdev = 5;
    screen_w = *w; screen_h = *h;
    char buf[64];
    int len = sprintf(buf, "%d %d", screen_w, screen_h);
    // let NWM resize the window and create the frame buffer
    write(fbctl, buf, len);
    while (1) {
      // 3 = evtdev
      int nread = read(3, buf, sizeof(buf) - 1);
      if (nread <= 0) continue;
      buf[nread] = '\0';
      if (strcmp(buf, "mmap ok") == 0) break;
    }
    close(fbctl);
  }
}

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
  int fd = _open("/dev/fb", 0, 0);

  size_t n = sizeof(uint32_t);
  size_t len = w * n;
  for (int canvas_row = y; canvas_row < y + h; canvas_row++) {
    int col = canvas_x + x;
    int row = canvas_y + canvas_row;
    size_t offset = (col + row * screen_w) * n;
    _lseek(fd, offset, SEEK_SET);
    _write(fd, pixels + (canvas_row - y) * w, w * n);
  }
  _close(fd);
}

void NDL_OpenAudio(int freq, int channels, int samples) {
}

void NDL_CloseAudio() {
}

int NDL_PlayAudio(void *buf, int len) {
  return 0;
}

int NDL_QueryAudio() {
  return 0;
}

int NDL_Init(uint32_t flags) {
  if (getenv("NWM_APP")) {
    evtdev = 3;
  }
  int disp = _open("/proc/dispinfo", 0, 0);
  char buf[64];
  int count = _read(disp, buf, sizeof(buf));
  _close(disp);
  screen_h = screen_w = 0;

  char token[64];
  char *token_ptr = token;
  char *buf_ptr = buf;
  while (buf_ptr - buf < count) {
    token_ptr = token;
    while(*buf_ptr == ' ' || *buf_ptr == '\n') buf_ptr++;
    while(*buf_ptr != ':' && *buf_ptr != '\0' && *buf_ptr != '\n')
      *token_ptr++ = *buf_ptr++;
    *token_ptr++ = '\0';
    if (strcmp(token, "WIDTH") == 0) {
      while(*buf_ptr < '1' || *buf_ptr > '9') buf_ptr++;      
      while(*buf_ptr >= '0' && *buf_ptr <= '9')
        screen_w = screen_w * 10 + *buf_ptr++ - '0';
    } else if (strcmp(token, "HEIGHT") == 0) {
      while(*buf_ptr < '1' || *buf_ptr > '9') buf_ptr++;      
      while(*buf_ptr >= '0' && *buf_ptr <= '9')
        screen_h = screen_h * 10 + *buf_ptr++ - '0';
    }
  }
  printf("\033[36mscreen width: %d, screen height: %d\033[0m\n", screen_w, screen_h);
  return 0;
}

void NDL_Quit() {
}
