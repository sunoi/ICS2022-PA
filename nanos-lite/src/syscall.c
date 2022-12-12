#include <common.h>
#include "syscall.h"

void do_syscall(Context *c) {
  uintptr_t a[4];
  a[0] = c->GPR1;

  switch (a[0]) {
		case 1:
			SYS_yield(c);
			break;
    default: panic("Unhandled syscall ID = %d", a[0]);
  }
}

void SYS_yield(Context *c) {
	c->GPRx = 0;
	yield();
}
