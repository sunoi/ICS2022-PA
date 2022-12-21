#include <NDL.h>
#include <SDL.h>
#include <string.h>
#include <assert.h>

#define keyname(k) #k,

static const char *keyname[] = {
  "NONE",
  _KEYS(keyname)
};

static char keyState[83] = {};

int SDL_PushEvent(SDL_Event *ev) {
  //panic("to be finished");
	assert(0);
	return 0;
}

int SDL_PollEvent(SDL_Event *ev) {
	static char buf[64];
	static char type[8];
	static char name[16];
	memset(buf, 0, sizeof(buf));

	if (NDL_PollEvent(buf, sizeof(buf)) == 0)
		return 0;
	
	memset(type, 0, sizeof(type));
	memset(name, 0, sizeof(name));
	sscanf(buf, "%s %s", type, name);
	//printf("type=%s, name=%s\n", type, name);
	int n = sizeof(keyname) / sizeof(char *);
	for (int i = 0; i < n; i++) {
		if (strcmp(name, keyname[i]) == 0) {
			ev->key.keysym.sym = i;
			break;
		}
	}

	if (strcmp(type, "kd") == 0) {
		ev->type = SDL_KEYDOWN;
		keyState[ev->key.keysym.sym] = 1;
	}
	else {
		ev->type = SDL_KEYUP;
		keyState[ev->key.keysym.sym] = 0;
	}
  return 1;
}

int SDL_WaitEvent(SDL_Event *event) {
	while(SDL_PollEvent(event) == 0);
	printf("reach here\n");
  return 1;
}

int SDL_PeepEvents(SDL_Event *ev, int numevents, int action, uint32_t mask) {
  return 0;
}

uint8_t* SDL_GetKeyState(int *numkeys) {
  return NULL;
}
