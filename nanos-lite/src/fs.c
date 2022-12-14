#include <fs.h>

typedef size_t (*ReadFn) (void *buf, size_t offset, size_t len);
typedef size_t (*WriteFn) (const void *buf, size_t offset, size_t len);

size_t ramdisk_read(void *buf, size_t offset, size_t len);
size_t ramdisk_write(const void *buf, size_t offset, size_t len);

typedef struct {
  char *name;
  size_t size;
  size_t disk_offset;
  ReadFn read;
  WriteFn write;
	size_t open_offset;
} Finfo;

enum {FD_STDIN, FD_STDOUT, FD_STDERR, FD_FB};

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
  [FD_STDIN]  = {"stdin", 0, 0, invalid_read, invalid_write, 0},
  [FD_STDOUT] = {"stdout", 0, 0, invalid_read, invalid_write, 0},
  [FD_STDERR] = {"stderr", 0, 0, invalid_read, invalid_write, 0},
#include "files.h"
};

int file_number = sizeof(file_table) / sizeof(Finfo);

void init_fs() {
  // TODO: initialize the size of /dev/fb
}

int fs_open(const char *pathname, int flags, int mdoe) {
	for (int i = 0; i < file_number; i++) {
		printf("%s\n", file_table[i].name);
		if (strcmp(pathname, file_table[i].name) == 0) {
			return i;
		}
	}
	return -1;
}

size_t fs_read(int fd, void *buf, size_t len) {
	size_t disk_offset = file_table[fd].disk_offset;
	size_t open_offset = file_table[fd].open_offset;
	size_t size = file_table[fd].disk_offset;
	
	printf("%d,%d,%d\n", disk_offset, open_offset, size);
	if (len > size - open_offset) 
		len = size - open_offset;

	ramdisk_read(buf, open_offset+disk_offset, len);
	file_table[fd].open_offset += len;
	return len;
}
size_t fs_write(int fd, const void *buf, size_t len) {
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
	return file_table[fd].open_offset;
}

int fs_close(int fd) {
	file_table[fd].open_offset = 0;
	return 0;
}

size_t fs_disk_offset(int fd) {
	return file_table[fd].disk_offset;
}
