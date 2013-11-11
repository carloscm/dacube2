
#include <stdio.h>
#include <stdlib.h>

#include "oldskool.h"

#include "mxm/mxmplay.h"

extern unsigned char module[];
static int mxm_started = 0;
#define MXM_DSOUND_DELAY -3

int os_mod_start(int argc, char** argv) {
#ifdef WIN32
	if (dsInit(GetForegroundWindow(), module)) {
#else
	if (ossInit(module)) {
#endif
		os_panic("Error initializing MXMPlay");
		return -1;
	}
	xmpPlay(0);
	mxm_started = 1;
	return 0;
}

void os_mod_end() {
	if (mxm_started == 0)
		return;
	xmpStop();
#ifdef WIN32
	dsClose();
#else
	ossClose();
#endif
}

int os_mod_get_order() {
	int r = (xmpGetPos() >> 8) & 0xff;
#ifdef WIN32
	int p = xmpGetPos() & 0xff;
	p += MXM_DSOUND_DELAY;
	if (p > 63)
		r++;
	if (p < 0)
		r--;
#endif
	return r;
}

int os_mod_get_row() {
	int r = xmpGetPos() & 0xff;
#ifdef WIN32
	return (r + MXM_DSOUND_DELAY) & 63;
#else
	return r;
#endif
}

