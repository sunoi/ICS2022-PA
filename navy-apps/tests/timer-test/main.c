#include <stdio.h>
#include <sys/time.h>
#include <unistd.h>
#include "/home/szz/.ssh/ics2022/navy-apps/libs/libndl/include/NDL.h"

void use_ndl() {
	NDL_Init(0);
	uint32_t next_msec = NDL_GetTicks();
	uint64_t msec = 0;
	while (1) {
		while (msec < next_msec) {
			msec = NDL_GetTicks();
		}
		msec = NDL_GetTicks();
		printf("0.5s later\n");
		next_msec += 5000;
	}
}

int main() {
	use_ndl();
	return 0;
}
