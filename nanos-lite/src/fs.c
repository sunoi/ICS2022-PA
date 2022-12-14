#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);
size_t serial_write(const void *buf, size_t offset, size_t len);
size_t events_read(void *buf, size_t offset, size_t len);
size_t dispinfo_read(void *buf, size_t offset, size_t len);
size_t fb_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
	size_t open_offset;
  ReadFn read;
  WriteFn write;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_EVENT, FD_FB, FD_DISP};

size_t invalid_read(void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

size_t invalid_write(const void *buf, size_t offset, size_t len) {
  panic("should not reach here");
  return 0;
}

/* This is the information about all files in disk. */
static Finfo file_table[] __attribute__((used)) = {
  [FD_STDIN]  = {"stdin", 0, 0, 0, invalid_read, invalid_write},
  [FD_STDOUT] = {"stdout", 0, 0, 0, invalid_read, serial_write},
  [FD_STDERR] = {"stderr", 0, 0, 0, invalid_read, serial_write},
	[FD_EVENT] = {"/dev/events", 0, 0, 0, events_read, invalid_write},
	[FD_FB] = {"/dev/fb", 0, 0, 0, invalid_read, fb_write},
	[FD_DISP] = {"/proc/dispinfo", 0, 0, 0, dispinfo_read, invalid_write},
#include "files.h"
};

int file_number = sizeof(file_table) / sizeof(Finfo) - 1;

void init_fs() {
  // TODO: initialize the size of /dev/fb
	file_table[FD_FB].size = 400 * 300 * 4;
	
}

int fs_open(const char *pathname, int flags, int mdoe) {
	//printf("\n\nto found file is %s\n", pathname);
	//printf("filenumber=%d\n", file_number);
	for (int i = 0; i < file_number; i++) {
		//printf("%s\n", file_table[i].name);
		if (strcmp(pathname, file_table[i].name) == 0) {
			return i;
		}
	}
	return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
	//printf("%s\n", file_table[fd].name);
	if (file_table[fd].read != NULL)
		return file_table[fd].read(buf, file_table[fd].open_offset, len);

	size_t disk_offset = file_table[fd].disk_offset;
	size_t open_offset = file_table[fd].open_offset;
	size_t size = file_table[fd].size;
	
	//printf("read_offset=%d\n", open_offset);
	//printf("fs_read = %d,%d,%d\n", disk_offset, open_offset, size);
	if (len > size - open_offset){
		len = size - open_offset;
	}
	ramdisk_read(buf, open_offset + disk_offset, len);
	file_table[fd].open_offset += len;
	return len;
}

size_t fs_write(int fd, const void *buf, size_t len) {
	if (file_table[fd].write != NULL)
		return file_table[fd].write(buf, file_table[fd].open_offset, len);

	size_t disk_offset = file_table[fd].disk_offset;
	size_t open_offset = file_table[fd].open_offset;
	size_t size = file_table[fd].size;
	
	if (len > size - open_offset)
		len = size - open_offset;

	ramdisk_write(buf, open_offset+disk_offset, len);
	file_table[fd].open_offset += len;
	return len;
}

size_t fs_lseek(int fd, size_t offset, int whence) {
	//printf("name=%s, whence=%d, offset=%d\n", file_table[fd].name, whence, offset);
	switch (whence) {
		case SEEK_SET:
			file_table[fd].open_offset = offset;
			break;
		case SEEK_CUR:
			file_table[fd].open_offset += offset;
			break;
		case SEEK_END:
			file_table[fd].open_offset = file_table[fd].size + offset;
			break;
		default:
			return -1;
	}
	//printf("%d\n", file_table[fd].open_offset);
	return file_table[fd].open_offset;
}

int fs_close(int fd) {
	//printf("close=%s\n", file_table[fd].name);
	if (file_table[fd].write == NULL) {
		file_table[fd].open_offset = 0;
		return 0;
	}
	else return -1;
}

size_t fs_disk_offset(int fd) {
	return file_table[fd].disk_offset;
}

