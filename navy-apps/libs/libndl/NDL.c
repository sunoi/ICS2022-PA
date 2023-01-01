#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <fcntl.h>
#include <sys/time.h>

static int evtdev = -1;
static int fbdev = -1;
static int dispdev = -1;
static int screen_w = 0, screen_h = 0;
static int width = 0, height = 0;

uint32_t NDL_GetTicks() {
	struct timeval t;
	gettimeofday(&t, NULL);
	return t.tv_sec * 1000 + t.tv_usec / 1000;
}

int NDL_PollEvent(char *buf, int len) {
	return read(evtdev, buf, len);
}

void NDL_OpenCanvas(int *w, int *h) {
	char buf[64];
	read(dispdev, buf, sizeof(buf));
	sscanf(buf, "WIDTH:%d\nHEIGHT:%d\n", &width, &height);
	if (*w == 0 && *h == 0) {
		*w = width;
		*h = height;
	}
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

static int flag = 0;
static int offset_x = 0;
static int offset_y = 0;

void NDL_DrawRect(uint32_t *pixels, int x, int y, int w, int h) {
	if (flag == 0) {
		offset_x = (width - w) / 2;
		offset_y = (height - h) / 2;
		flag = 1;
	}

	x += offset_x;
	y += offset_y;

	uint32_t color[w];
	for (int i = 0; i < h; i++) {
		for (int j = 0; j < w; j++) {
			color[j] = pixels[i*w+j];
		}
		lseek(fbdev, (x+(y+i)*width)*4, SEEK_SET);
		write(fbdev, color, w);
	}

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
	evtdev = open("/dev/events", 0, 0);
	fbdev = open("/dev/fb", 0, 0);
	dispdev = open("/proc/dispinfo", 0, 0);
	//printf("dispdev=%d\n", dispdev);
  return 0;
}

void NDL_Quit() {
	close(evtdev);
	close(dispdev);
}
