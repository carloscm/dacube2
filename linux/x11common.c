
/* common code to x11 display */

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include "x11int.h"

static int already_open = 0;

int x11_open (x11_data *d)
{
	XPixmapFormatValues *pfv;
	int i, n;

	if (already_open)
		return 0;

	d->display = XOpenDisplay (NULL);
  
	if (d->display == NULL) {
		//__wr_text ("Failed to open X11 display");
		return 1;
	}

	d->screen = DefaultScreen (d->display);
	d->depth  = DefaultDepth (d->display, d->screen);

	d->pixdepth = d->depth;
	pfv = XListPixmapFormats (d->display, &n);
 
	for (i = 0; i < n; i++) {
		if (pfv[i].depth == d->depth) {
			d->pixdepth = pfv[i].bits_per_pixel;
			break;
		}
	}
	XFree(pfv);

	if ((d->pixdepth != 16) && (d->pixdepth != 24) && (d->pixdepth != 32) &&
		(d->pixdepth != 15) ) {
		//__wr_text ("Non suported color depth");
		return 1;
	}

	d->rootwin  = RootWindow (d->display, d->screen);
	d->gc       = DefaultGC (d->display, d->screen);
	d->black    = BlackPixel (d->display, d->screen);
	d->visual   = DefaultVisual (d->display, d->screen);

	//D2("(x11_open) server depth is %d, pixmap is %d\n", d->depth, d->pixdepth);

	already_open = 1;

	return 0;
}
