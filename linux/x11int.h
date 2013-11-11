
#ifndef X11INT_H
#define X11INT_H

typedef struct _x11_data {
	unsigned int depth;
	int pixdepth;
	int screen;
	Display *display;
	Window  rootwin;
	GC      gc;
	unsigned int black;
	Visual *visual;
} x11_data;

extern int x11_open (x11_data *d);

extern int vidmode_init (x11_data *);
extern void vidmode_end (x11_data *);
extern int vidmode_set_mode (x11_data *, int width, int height);

#endif

