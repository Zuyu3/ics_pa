#include <NDL.h>
#include <SDL.h>
#include <string.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
  char buf[50];
  if(!NDL_PollEvent(buf, 50))
    return 0;
  ev->key.type = strncmp(buf, "ku", 2) ? SDL_KEYDOWN: SDL_KEYUP;
  for(int i = 0; i < sizeof(keyname) / sizeof(char *); i++) {
    if(!strcmp(keyname[i], buf + 3)) {
      ev->key.keysym.sym = i;
      return 1;
    }
  }
}

int SDL_WaitEvent(SDL_Event *event) {
  char buf[50];
  while(!NDL_PollEvent(buf, 50)) {
  }
  //printf("%s\n", buf);
  event->key.type = strncmp(buf, "ku", 2) ? SDL_KEYDOWN: SDL_KEYUP;
  for(int i = 0; i < sizeof(keyname) / sizeof(char *); i++) {
    if(!strcmp(keyname[i], buf + 3)) {
      event->key.keysym.sym = i;
      return 1;
    }
  }
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
