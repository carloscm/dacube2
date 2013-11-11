
#include "oldskool.h"

#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include <stdlib.h>
#include <string.h>

#include "win32/win.h"

static int start_milis = 0;
static int app_hz = 0;

void os_panic(char *msg) {
	MessageBox(NULL, msg, "ccm sux", MB_OK | MB_ICONSTOP);
}

int os_ticks_init(int hz) {
	start_milis = GetTickCount();
	app_hz = hz;
	return 0;
}

void os_ticks_reset() {
	start_milis = GetTickCount();
}

int os_ticks_get() {
	int milis = GetTickCount() - start_milis;
	return milis / (1000 / app_hz);
}

int os_ticks_end() {
	return 0;
}

typedef struct os_color_t_ {
	unsigned char r, g, b, pad;
} os_color_t;
static os_color_t os_fb_palette[256];
static os_byte_t* os_byte_fb = NULL;

int os_fb_init(int argc, char** argv) {
	int i;
	if (os_byte_fb != NULL)
		return 1;
	os_byte_fb = (os_byte_t*) malloc(640*480*4);
	for (i = 0; i < 256; ++i) {
		os_fb_palette[i].r = 0;
		os_fb_palette[i].g = 0;
		os_fb_palette[i].b = 0;
	}
	i = ddraw_display_init (640, 480, "dacube2");
	if (i == -1) {
		os_fb_end();
		return -1;
	}
	return i;
}

void os_fb_end() {
	if (os_byte_fb == NULL)
		return;
	free(os_byte_fb);
	os_byte_fb = NULL;
	ddraw_display_close();
}

os_byte_t* os_fb_get_buffer() {
	return os_byte_fb;
}

void os_fb_set_palette(int entry, int red, int green, int blue) {
	os_fb_palette[entry].r = red;
	os_fb_palette[entry].g = green;
	os_fb_palette[entry].b = blue;
}

int os_fb_get_palette_red(int entry) {
	return os_fb_palette[entry].r;
}

extern int os_fb_get_palette_green(int entry) {
	return os_fb_palette[entry].g;
}

extern int os_fb_get_palette_blue(int entry) {
	return os_fb_palette[entry].b;
}

void os_fb_clear(int entry) {
	if (os_byte_fb)
		memset(os_byte_fb, entry, 640*480);
}

void os_fb_clear_full(int entry) {
	if (os_byte_fb)
		memset(os_byte_fb, entry, 640*480*4);
}

int os_fb_swap_other_buffer(void* otherbuf) {
	void* rbuf = NULL;
	int bpp = -1;
	int pitch = -1;
	unsigned char* appbuf = otherbuf, *pitchadj = NULL;
	int r;
	
	if (appbuf == NULL)
		return 1;

	ddraw_display_lock(&rbuf, &bpp, &pitch);

	if (bpp == 32) {
		unsigned int* buf = rbuf, pixel;
		int i, j;
		for (j = 0; j < 480; ++j) {
			for (i = 0; i < 640; ++i, ++appbuf, ++buf) {
				pixel  = os_fb_palette[*appbuf].b << 2;
				pixel |= (os_fb_palette[*appbuf].g << 2) << 8;
				pixel |= (os_fb_palette[*appbuf].r << 2) << 16;
				*buf = pixel;
			}
			pitchadj = buf;
			pitchadj += pitch - 640*4;
			buf = pitchadj;
		}
	} else
	if (bpp == 16) {
		unsigned short* buf = rbuf, pixel;
		int i, j;
		for (j = 0; j < 480; ++j) {
			for (i = 0; i < 640; ++i, ++appbuf, ++buf) {
				pixel  = os_fb_palette[*appbuf].b >> 1;
				pixel |= os_fb_palette[*appbuf].g << 5;
				pixel |= (os_fb_palette[*appbuf].r >> 1) << 11;
				*buf = pixel;
			}
			pitchadj = buf;
			pitchadj += pitch - 640*2;
			buf = pitchadj;
		}
	} else
	if (bpp == 15) {
		unsigned short* buf = rbuf, pixel;
		int i, j;
		for (j = 0; j < 480; ++j) {
			for (i = 0; i < 640; ++i, ++appbuf, ++buf) {
				pixel  = os_fb_palette[*appbuf].b >> 1;
				pixel |= (os_fb_palette[*appbuf].g >> 1) << 5;
				pixel |= (os_fb_palette[*appbuf].r >> 1) << 10;
				*buf = pixel;
			}
			pitchadj = buf;
			pitchadj += pitch - 640*2;
			buf = pitchadj;
		}
	} else
	if (bpp == 24) {
		unsigned char* buf = rbuf;
		int i, j;
		for (j = 0; j < 480; ++j) {
			for (i = 0; i < 640; ++i, ++appbuf) {
				*buf = os_fb_palette[*appbuf].b << 2; ++buf;
				*buf = os_fb_palette[*appbuf].g << 2; ++buf;
				*buf = os_fb_palette[*appbuf].r << 2; ++buf;
			}
			pitchadj = buf;
			pitchadj += pitch - 640*3;
			buf = pitchadj;
		}
	} else
	if (bpp == 8) {
		unsigned char* buf = rbuf;
		int j;
		for (j = 0; j < 480; ++j) {
			memcpy(buf, appbuf, 640);
			buf += pitch;
			appbuf += 640;
		}
		//ddraw_display_set_palette(os_fb_palette);
	}

	r = ddraw_display_unlock();
	if (bpp == 8) {
		ddraw_display_set_palette(os_fb_palette);
	}
	return r;
}

int os_fb_swap_buffers() {
	return os_fb_swap_other_buffer(os_byte_fb);
}

