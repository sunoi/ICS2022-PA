#include <common.h>
#include "syscall.h"
#include <time.h>
#include "fs.h"
#include <sys/time.h>
#include <proc.h>

int fs_open(const char *pathname, int flags, int mode);
size_t fs_read(int fd, void *buf, size_t len);
size_t fs_write(int fd, const void *buf, size_t len);
size_t fs_lseek(int fd, size_t offset, int whence);
int fs_close(int fd);

void naive_uload(PCB *pcb, const char *filename);

int execve(const char *filename, char* const arg[], char* const envp[]) {
	int fd = fs_open(filename, 0, 0);
	if (fd == -1) {
		return -1;
	}
	else {
		fs_close(fd);
	}
	naive_uload(NULL, filename);
	return 0;
}

void sys_yield(Context *c) {
	yield();
	c->GPRx = 0;
}

void sys_exit(Context *c) {
	int status = c->GPR2;
	if (status == 0) {
		execve("/bin/nterm", 0, 0);
	}
	else {
		halt(status);
	}
}

void sys_write(Context *c) {
	int fd = c->GPR2;
	char* buf = (char*)c->GPR3;
	size_t count = c->GPR4;
	c->GPRx = fs_write(fd, (const void*)buf, count);
}

void sys_brk(Context *c) {
	//intptr_t addr = c->GPR2;
	c->GPRx = 0;
	//printf("%d\n", addr);
}

void sys_open(Context *c) {
	const char* pathname = (const char*)c->GPR2;
	int flags = c->GPR3;
	int mode = c->GPR4;
	//printf("open_pathname = %s\n", pathname);
	c->GPRx = fs_open(pathname, flags, mode);
	//printf("fd = %d\n", c->GPRx);
}

void sys_read(Context *c) {
	int fd = c->GPR2;
	int count = c->GPR4;
	char *buf = (char*)c->GPR3;
	c->GPRx = fs_read(fd, (void*)buf, count);
}

void sys_lseek(Context *c) {
	int fd = c->GPR2;
	int offset = c->GPR3;
	int whence = c->GPR4;

	c->GPRx = fs_lseek(fd, offset, whence);
}

void sys_close(Context *c) {
	int fd = c->GPR2;
	c->GPRx = fs_close(fd);
}

void sys_execve(Context *c) {
	const char* filename = (const char*)c->GPR2;
	char* *arg = (char**)c->GPR3;
	char* const* envp = (char* const*)c->GPR4;
	//printf("filename=%s\n", filename);
	c->GPRx = execve(filename, arg, envp);
}

void sys_gettimeofday(Context *c) {
	struct timeval* tv = (struct timeval*)c->GPR2;
	// struct timezone *tz = (struct timezone*)c->GPR3;
	uint64_t us = io_read(AM_TIMER_UPTIME).us;
	//printf("%d\n", us);
	tv->tv_sec = us / 1000000;
	tv->tv_usec = us - tv->tv_sec * 1000000;

	c->GPRx = 0;
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
	//printf("a[0]=%d\n", a[0]);
  switch (a[0]) {
		case 0:
			sys_exit(c);break;
		case 1:
			sys_yield(c);break;
		case 2:
			sys_open(c);break;
		case 3:
			sys_read(c);break;
		case 4:
			sys_write(c);break;
		case 7:
			sys_close(c);break;
		case 8:
			sys_lseek(c);break;
		case 9:
			sys_brk(c);break;
		case 13:
			sys_execve(c);break;
		case 19:
			sys_gettimeofday(c);break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
