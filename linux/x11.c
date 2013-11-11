
#include <stdlib.h>
#include <stdio.h>
#include <assert.h>

#include <X11/X.h>
#include <X11/Xlib.h>
#include <X11/Xutil.h>
#include <X11/keysym.h>

#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

#include "x11.h"
#include "x11int.h"
#include "config.h"

typedef struct _x11_window {
	Window  win;
	XImage *image;
	int    w, h;
	void   *buf;
	int with_shm;
	XShmSegmentInfo shm_seg_info;
	int	active;
	int	fullscreen;
} x11_window;

static Atom x11_close_atom; 

static x11_data server;

static int x11_quit_condition = 0;

static int x11_initimage(XImage *image);

void x11_event_proc(XEvent *e) {
	KeySym sym;
	switch (e->type) {
		case KeyPress:
			if (e->xkey.type == KeyRelease)
				break;

			sym = XLookupKeysym(&(e->xkey), 0);
			if (sym == XK_Escape) {
				x11_quit_condition = 1;
			}
		break;

		case ClientMessage:
			if ( (e->xclient.format == 32) &&
			     ((Atom)e->xclient.data.l[0] == x11_close_atom) )
				x11_quit_condition = 1;
		default:
		break;
	}
}


void x11_update_window() {
	XEvent e;
	int n = XPending(server.display), i = 0;
	while (i < n) {
		XNextEvent(server.display, &e);
		x11_event_proc(&e);
		i++;
	}
}

static x11_window* da_window = NULL;

static int x11_display_init_proto(unsigned int w, unsigned int h, char *name, int argc, char** argv, int fullscreen) {
	static unsigned char bm_no_data[] = { 0,0,0,0, 0,0,0,0  };
	x11_window *new_window = NULL;
	XSizeHints size_hints;
	Window win;
	void *buf;
	XImage *image;
	XShmSegmentInfo *shm_seg_info;
	int with_shm, result;

	if (da_window != NULL)
		return 1;

	if (x11_open(&server) != 0)
		return 1;

	x11_close_atom = XInternAtom(server.display, "WM_DELETE_WINDOW", False);

	new_window = malloc(sizeof(x11_window));
	assert(new_window != NULL);

	shm_seg_info = &(new_window->shm_seg_info);

	if (fullscreen) {
		if (vidmode_init(&server)) {
			printf("WARNING: Cannot init XF86VidMode extension. Reverting to windowed.\n");
			fullscreen = 0;
		} else if (vidmode_set_mode(&server, w, h)) {
			printf("WARNING: Cannot change video mode. Reverting to windowed.\n");
			fullscreen = 0;
		}
	}

	if (fullscreen) {
		XSetWindowAttributes att;
		Cursor no_ptr;
		Pixmap bm_no;
		XColor black, dummy;
		Colormap colormap;
		att.background_pixel = 0;
		att.border_pixel= 0;
		att.override_redirect = True;
		win = XCreateWindow(server.display, server.rootwin, 0, 0, w, h, 0, server.depth,
			CopyFromParent, server.visual,
			CWOverrideRedirect | CWBackPixel | CWBorderPixel, &att);
		XRaiseWindow(server.display, win);
		colormap = DefaultColormap(server.display, server.screen);
		XAllocNamedColor(server.display, colormap, "black", &black, &dummy);
		bm_no = XCreateBitmapFromData(server.display, win, bm_no_data, 8, 8);
		no_ptr = XCreatePixmapCursor(server.display, bm_no, bm_no, &black, &black, 0, 0);
		XDefineCursor(server.display, win, no_ptr);
		XFreeCursor(server.display, no_ptr);
		if (bm_no != None)
			XFreePixmap(server.display, bm_no);
	} else {
		win = XCreateSimpleWindow(server.display, server.rootwin, 5, 5, w, h, 0,
			server.black, server.black);
		size_hints.flags      = PSize | PMinSize | PMaxSize;
		size_hints.min_width  = w;
		size_hints.max_width  = w;
		size_hints.min_height = h;
		size_hints.max_height = h;
		XSetStandardProperties(server.display, win, name, name, None,
			0, 0, &size_hints); 
	}
	
	XSetWMProtocols(server.display, win, &x11_close_atom, 1);

	XSelectInput(server.display, win, StructureNotifyMask | KeyPressMask | ButtonPressMask |
		ButtonReleaseMask | PointerMotionMask);

	XMapWindow(server.display, win);
	for (;;) {
		XEvent e;
		XNextEvent(server.display, &e);
		if (e.type == MapNotify)
			break;
	}

	XFlush(server.display);
	XSync(server.display, False);

	if (fullscreen) {
                XGrabKeyboard(server.display, win, True, GrabModeAsync, GrabModeAsync,
			CurrentTime);
		XGrabPointer(server.display, win, True, 0,GrabModeAsync, GrabModeAsync,
			win, None, CurrentTime);
		//XSetInputFocus(server.display, win, RevertToNone, CurrentTime);
	}

	XFlush(server.display);
	XSync(server.display, False);

	if (XShmQueryExtension(server.display)) {
		image = XShmCreateImage(server.display, server.visual, server.depth, ZPixmap,
			NULL, shm_seg_info, w, h);
		if (image == NULL) {
			return 1;
		}
	
		shm_seg_info->shmid = shmget(IPC_PRIVATE, image->bytes_per_line * image->height,
			IPC_CREAT | 0777);
		if (shm_seg_info->shmid == -1) {
			return 1;
		}  
	
		shm_seg_info->shmaddr = (char*) shmat(shm_seg_info->shmid, 0, 0);
		shm_seg_info->readOnly = False;
		image->data = shm_seg_info->shmaddr;
		if (shm_seg_info->shmaddr == (char*) -1) {
			return 1;
		}

		if (!XShmAttach(server.display, shm_seg_info)) {
			return 1;
		}
	
		XSync(server.display, 0);
	
		with_shm = 1;
		buf = shm_seg_info->shmaddr;
	} else {
		buf = malloc((w * h * server.pixdepth) / 8);
		assert(buf != NULL);
		image = XCreateImage(server.display, server.visual, server.depth, ZPixmap,
			0, (char *) buf, w, h, server.pixdepth, 0);
		with_shm = 0;
	}

	if (x11_initimage(image) != 0)
		return 1;

	XSync(server.display, 0);

	new_window->win = win;
	new_window->image = image;
	new_window->w = w;
	new_window->h = h;
	new_window->buf = buf;
	new_window->with_shm = with_shm;
	new_window->active = 1;
	new_window->fullscreen = fullscreen;

	da_window = new_window;

	return 0;
}

int x11_display_init(unsigned int w, unsigned int h, char *name, int argc, char** argv) {
	int result = config_dialog(name, argc, argv);
	int x11res = -1;
	if (result == -1)
		return -1;
	if (result & C_WIN) 
		x11res = x11_display_init_proto(w, h, name, argc, argv, 0);
	else
		x11res = x11_display_init_proto(w, h, name, argc, argv, 1);
	if (x11res == -1)
		return -1;
	return result & C_OLD ? 1 : 0;
}


int x11_display_close() {
	x11_window* win;
	if (da_window == NULL)
		return 0;

	win = da_window;

	if (win->with_shm == 1) {
		XShmDetach(server.display, &(win->shm_seg_info));
		XSync(server.display, 0);
	}

	XDestroyImage(win->image);

	if (win->with_shm == 1) {
		shmdt(win->shm_seg_info.shmaddr);
		shmctl(win->shm_seg_info.shmid, IPC_RMID, 0);
	}

	XUnmapWindow(server.display, win->win);
	XSync(server.display, 0);

	XDestroyWindow(server.display, win->win);

	if (win->fullscreen) {
		vidmode_end(&server);
	}

	free(win);

	da_window = NULL;

	return 0;
}

int x11_get_bpp() {
	return server.pixdepth;
}

char* x11_get_buffer() {
	if (da_window == NULL)
		return NULL;
	return da_window->image->data;
}

int x11_display_swap() {
	x11_window *win;
	if (da_window == NULL)
		return 1;
	win = da_window;

	if (win->with_shm == 0)
		XPutImage(server.display, win->win, server.gc, win->image, 0, 0, 0, 0,
			(unsigned int) win->image->width, (unsigned int) win->image->height);
	else
		XShmPutImage(server.display, win->win, server.gc, win->image, 0, 0, 0, 0,
			(unsigned int) win->image->width, (unsigned int) win->image->height, 0);

	XSync(server.display, 0);
	x11_update_window();

	return x11_quit_condition;
}

static int x11_initimage(XImage *image) {
	if (!image) {
		return 1;
	}

	if (image->bits_per_pixel != server.pixdepth) {
		return 1;
	}

	if ((image->bits_per_pixel!=16) && (image->bits_per_pixel!=32) &&
	(image->bits_per_pixel!=15) && (image->bits_per_pixel!=24)) {
		return 1;
	}

	return 0;
}

