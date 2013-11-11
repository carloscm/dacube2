
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/times.h>
#include <sys/time.h>
#include <unistd.h>

#include "oldskool.h"
#include "linux/x11.h"

void os_panic(char *msg) {
	printf("%s\n", msg);
}

static int os_get_milis() {
	struct timeval tt;
	gettimeofday(&tt, NULL);
	return tt.tv_sec*1000 + tt.tv_usec/1000;
}

static int start_milis = 0;
static int app_hz = 0;

int os_ticks_init(int hz) {
	start_milis = os_get_milis();
	app_hz = hz;
	return 0;
}

int os_ticks_get() {
	int milis = os_get_milis() - start_milis;
	//printf("tick %d\n", milis / (1000 / app_hz));
	return milis / (1000 / app_hz);
}

int os_ticks_end() {
	return 0;
}

typedef struct os_color_t_ {
	int r, g, b;
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
	return x11_display_init(640, 480, "dacube2", argc, argv);
}

void os_fb_end() {
	if (os_byte_fb == NULL)
		return;
	free(os_byte_fb);
	os_byte_fb = NULL;
	x11_display_close();
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

int os_fb_swap_other_buffer(void* otherbuf) {
	void* rbuf = x11_get_buffer();
	int bpp = x11_get_bpp();
	unsigned char* appbuf = otherbuf;
	
	if (appbuf == NULL)
		return 1;

	if (bpp == 32) {
		unsigned int* buf = rbuf, pixel;
		int i;
		for (i = 0; i < 640*480; ++i, ++appbuf, ++buf) {
			pixel  = os_fb_palette[*appbuf].b << 2;
			pixel |= (os_fb_palette[*appbuf].g << 2) << 8;
			pixel |= (os_fb_palette[*appbuf].r << 2) << 16;
			*buf = pixel;
		}
	}  else
	if (bpp == 16) {
		unsigned short* buf = rbuf, pixel;
		int i;
		for (i = 0; i < 640*480; ++i, ++appbuf, ++buf) {
			pixel  = os_fb_palette[*appbuf].b >> 1;
			pixel |= os_fb_palette[*appbuf].g << 5;
			pixel |= (os_fb_palette[*appbuf].r >> 1) << 11;
			*buf = pixel;
		}
	}
	if (bpp == 15) {
		unsigned short* buf = rbuf, pixel;
		int i;
		for (i = 0; i < 640*480; ++i, ++appbuf, ++buf) {
			pixel  = os_fb_palette[*appbuf].b >> 1;
			pixel |= (os_fb_palette[*appbuf].g >> 1) << 5;
			pixel |= (os_fb_palette[*appbuf].r >> 1) << 10;
			*buf = pixel;
		}
	} else
	if (bpp == 24) {
		unsigned char* buf = rbuf;
		int i;
		for (i = 0; i < 640*480; ++i, ++appbuf) {
			*buf = os_fb_palette[*appbuf].b; ++buf;
			*buf = os_fb_palette[*appbuf].g; ++buf;
			*buf = os_fb_palette[*appbuf].r; ++buf;
		}
	}

	return x11_display_swap();
}

int os_fb_swap_buffers() {
	return os_fb_swap_other_buffer(os_byte_fb);
}

