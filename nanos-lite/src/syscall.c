#include <common.h>
#include "syscall.h"

void sys_yield(Context *c) {
	yield();
	c->GPRx = 0;
}

void sys_exit(Context *c) {
	int status = c->GPR2;
	halt(status);
}

void sys_write(Context *c) {
	int fd = c->GPR2;
	if (fd == 1 || fd == 2) {
		char* buf = (char*)c->GPR3;
		size_t len = c->GPR4;
		//printf("len=%d\n", len);
		for (size_t i = 0; i < len; i++) {
			putch(*buf);
			buf++;
		}
		c->GPRx = len;
	}
}

void sys_brk(Context *c) {
	intptr_t addr = c->GPR2;
	c->GPRx = 0;
	printf("%d\n", addr);
	return;
}


void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
	printf("%d\n", a[0]);
  switch (a[0]) {
		case 0:
			sys_yield(c);
			break;
		case 1:
			sys_exit(c);
			break;
		case 4:
			sys_write(c);
			break;
		case 9:
			sys_brk(c);
			break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
