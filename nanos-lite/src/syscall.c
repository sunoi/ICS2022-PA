#include <common.h>
#include "syscall.h"

void SYS_yield(Context *c) {
	yield();
	c->GPRx = 0;
	c->mepc += 4;
}

void SYS_exit(Context *c) {
	int status = c->GPR2;
	halt(status);
}

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
	//printf("a[0]=%d\n", a[0]);
  switch (a[0]) {
		case 0:
			SYS_yield(c);
			break;
		case 1:
			SYS_exit(c);
			break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
