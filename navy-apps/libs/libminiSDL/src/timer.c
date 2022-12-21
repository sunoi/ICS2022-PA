#include <NDL.h>
#include <sdl-timer.h>
#include <stdio.h>

SDL_TimerID SDL_AddTimer(uint32_t interval, SDL_NewTimerCallback callback, void *param) {
  return NULL;
}

int SDL_RemoveTimer(SDL_TimerID id) {
  return 1;
}

uint32_t SDL_GetTicks() {
	struct timeval t;
	gettimeofday(&t, NULL);
	assert(0);
	return t.tv_sec * 1000 + t.tv_usec / 1000;
}

void SDL_Delay(uint32_t ms) {
}
