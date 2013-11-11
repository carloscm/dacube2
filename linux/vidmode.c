
#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <X11/extensions/xf86vmode.h>

#include "x11int.h"

static XF86VidModeModeInfo **modesinfo = NULL;
static int modecount = -1;

int vidmode_init(x11_data *d) {
	int t1 = 0, t2 = 0;
	if (!XF86VidModeQueryExtension(d->display, &t1, &t2)) {
		return 1;
	}
	XF86VidModeGetAllModeLines(d->display, d->screen, &modecount, &modesinfo);
	return 0;
}

static int vidmode_match_mode(int w, int h) {
	int i;
	for (i = 0; i < modecount; ++i) {
		if(modesinfo[i]->hdisplay == w && modesinfo[i]->vdisplay == h)
			return i;
	}
	return -1;
}

int vidmode_set_mode(x11_data *d, int width, int height) {
	Bool r;
	int mode;

	mode = vidmode_match_mode(width, height);
	if (mode == -1)
		return 1;

	XF86VidModeSwitchToMode(d->display, d->screen, modesinfo[mode]);
	r = XF86VidModeSwitchToMode(d->display, d->screen, modesinfo[mode]);
	r |= XF86VidModeSetViewPort(d->display, d->screen, 0, 0);

	XFlush(d->display);

	return (!r) ? 1 : 0;
}

void vidmode_end(x11_data *d) {
	if (modecount > 0) {
		XF86VidModeSwitchToMode(d->display, d->screen, modesinfo[0]);
		XF86VidModeSwitchToMode(d->display, d->screen, modesinfo[0]);
	        XFree(modesinfo);
	}
	modesinfo = NULL;
	modecount = -1;
}

