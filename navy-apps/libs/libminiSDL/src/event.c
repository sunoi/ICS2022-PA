#include <NDL.h>
#include <SDL.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

int SDL_PushEvent(SDL_Event *ev) {
  panic("to be finished");
	return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
	char buf[64];
	char type[8];
	char name[16];

	if (!NDL_PollEvent(buf, sizeof(buf)))
		return 0;
	
	sscanf(buf, "%s %s", type, name);
	int n = sizeof(keyname) / sizeof(char *);
	for (int i = 0; i < n; i++) {
		if (strcmp(name, keyname[i]) == 0) {
			event->key.keysym.sym = i;
			break;
		}
	}

	if (strcmp(type, "ku") == 0) {
		event->type = SDL_KEYUP;
		keyState[event->key.keysym.sym] = 0;
	}
	else {
		event->type = SDL_KEYDOWN;
		keyState[event->key.keysym.sym] = 1;
	}
  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
	while(SDL_PollEvent(event) == 0);
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
