#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static char buf[128];

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  return 0;
}

int SDL_WaitEvent(SDL_Event *event) {
  int count = NDL_PollEvent(buf, sizeof(buf));
  if (count == 0)
    return 0;
  char key[20];
  char *ptr = buf;
  while (*ptr != 'k') ptr++;
  switch (*++ptr) {
    case 'd' : event->type = SDL_KEYDOWN; break;
    case 'u' : event->type = SDL_KEYUP; break;
  }
  while (*ptr != ' ') ptr++;
  ptr++;
  int i;
  for (i = 0; *ptr != '\n'; i++)
    key[i] = *ptr++;
  key[i] = '\0';
  for (i = 0; i < SDLK_KEY_NUM; i++) {
    if (strcmp(key, keyname[i]) == 0) {
      event->key.keysym.sym = i;
      break;
    }
  }
  return 0;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
