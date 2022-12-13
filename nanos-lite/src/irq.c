#include <common.h>
#include <am.h>
#include <proc.h>

void do_syscall(Context *c);

static Context* do_event(Event e, Context* c) {
	printf("%d\n", e.event);
	switch (e.event) {
		case EVENT_YIELD:
			Log("Trriger yield.");
			break;
		case EVENT_SYSCALL:
			Log("Trriger syscall");
			do_syscall(c);
			break;
    default: panic("Unhandled event ID = %d", e.event);
  }

  return c;
}

void init_irq(void) {
  Log("Initializing interrupt/exception handler...");
  cte_init(do_event);
}
