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
/*
void sys_write(Context *c) {
	
}*/

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;
  switch (a[0]) {
		case 0:
			sys_yield(c);
			break;
		case 1:
			sys_exit(c);
			break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}
