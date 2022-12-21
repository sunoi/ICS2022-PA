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
  char *str = (char*)buf;
	for (size_t i = 0; i < len; i++) {
		putch(*str);
		str++;
	}
	return len;
}

size_t events_read(void *buf, size_t offset, size_t len) {
  AM_INPUT_KEYBRD_T ev = io_read(AM_INPUT_KEYBRD);
	assert(0);
	if (ev.keycode == AM_KEY_NONE) return 0;

	if (ev.keydown == true) {
		sprintf(buf, "kd %s", keyname[ev.keycode]);
	}
	else {
		sprintf(buf, "ku %s", keyname[ev.keycode]);
	}
	return strlen(buf);
}

size_t dispinfo_read(void *buf, size_t offset, size_t len) {
	int width = io_read(AM_GPU_CONFIG).width;
	int height = io_read(AM_GPU_CONFIG).height;
	sprintf(buf, "WIDTH:%d\nHEIGHT:%d\n", width, height);
	//printf("width=%d, height=%d\n");
	return strlen(buf);
}

size_t fb_write(const void *buf, size_t offset, size_t len) {
  int width = io_read(AM_GPU_CONFIG).width;
	//printf("offset=%d, len=%d\n", offset, len);
	int t = offset / 4;
	int y = t / width;
	int x = t % width;
	
	io_write(AM_GPU_FBDRAW, x, y, (void*)buf, len, 1, true);
	return 0;
}

void init_device() {
  Log("Initializing devices...");
  ioe_init();
}
