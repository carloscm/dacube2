
#include <stdio.h>
#include <stdlib.h>

#include "oldskool.h"

extern unsigned char module[];
static int mxm_started = 0;
#define MXM_DSOUND_DELAY -3

int os_mod_start(int argc, char** argv) {
	return 0;
}

void os_mod_end() {
}

int os_mod_get_order() {
	return 0;
}

int os_mod_get_row() {
	return 0;
}

