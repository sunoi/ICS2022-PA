#include <common.h>
#include "syscall.h"

void sys_yield(Context *c) {
	yield();
	c->GPRx = 0;
	c->mepc += 4;
}

void sys_exit(Context *c) {
	int status = c->GPR2;
	halt(status);
}

void sys_write(Context *c) {
	int fd = c->GPR2;
	if (fd == 1 || fd == 2) {
		intptr_t buf = c->GPR3;
		int len = c->GPR4;
		for (int i = 0; i < len; i++) {
			putch(buf);
			buf++;
		}
	}
	c->GPRx = 0;
	assert(0);
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
	printf("a[0]=%d\n", a[0]);
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
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
