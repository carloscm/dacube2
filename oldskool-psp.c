
#include <pspkernel.h>
#include <pspdisplay.h>
#include <pspdebug.h>
#include <stdlib.h>
#include <stdio.h>
#include <math.h>
#include <string.h>
#include <time.h>

#include <pspgu.h>

#include "oldskool.h"


PSP_MODULE_INFO("dacube2", 0, 1, 1);
PSP_MAIN_THREAD_ATTR(THREAD_ATTR_USER);

#define printf	pspDebugScreenPrintf

#define SLICE_SIZE 64 // change this to experiment with different page-cache sizes

static unsigned int __attribute__((aligned(16))) list[262144];

int done = 0;
/* Exit callback */
int exit_callback(void)
{
	done = 1;
	return 0;
}

/* Callback thread */
int CallbackThread(SceSize args, void *argp)
{
	int cbid;

	cbid = sceKernelCreateCallback("Exit Callback", exit_callback, NULL);
	sceKernelRegisterExitCallback(cbid);

	sceKernelSleepThreadCB();

	return 0;
}

/* Sets up the callback thread and returns its thread id */
int SetupCallbacks(void)
{
	int thid = 0;

	thid = sceKernelCreateThread("update_thread", CallbackThread, 0x11, 0xFA0, 0, 0);
	if(thid >= 0)
	{
		sceKernelStartThread(thid, 0, 0);
	}

	return thid;
}

// FB. la dacube quiere 640x480 + margen...
static unsigned short __attribute__((aligned(16))) pixels[512*600+512*100];

struct Vertex
{
	unsigned short u, v;
	unsigned short color;
	short x, y, z;
};


float nopf(float f) {
	return f;
}

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
	return 0;
}

int os_ticks_end() {
	return 0;
}

typedef struct os_color_t_ {
	int r, g, b;
} os_color_t;
static os_color_t os_fb_palette[256];
static os_byte_t* os_byte_fb = NULL;

static int val;

int os_fb_init(int argc, char** argv) {
	int i;
	if (os_byte_fb != NULL)
		return 1;
	//os_byte_fb = //(os_byte_t*) malloc(640*480*4);
	os_byte_fb = pixels;

	for (i = 0; i < 256; ++i) {
		os_fb_palette[i].r = 0;
		os_fb_palette[i].g = 0;
		os_fb_palette[i].b = 0;
	}

	pspDebugScreenInit();
	SetupCallbacks();

	sceGuInit();

	// setup
	sceGuStart(0,list);
	sceGuDrawBuffer(GE_PSM_4444,(void*)0,512);
	sceGuDispBuffer(480,272,(void*)0x88000,512);
	sceGuDepthBuffer((void*)0x110000,512);
	sceGuOffset(2048 - (480/2),2048 - (272/2));
	sceGuViewport(2048,2048,480,272);
	sceGuDepthRange(0xc350,0x2710);
	sceGuScissor(0,0,480,272);
	sceGuEnable(GU_STATE_SCISSOR);
	sceGuFrontFace(GE_FACE_CW);
	sceGuEnable(GU_STATE_TEXTURE);
	sceGuClear(GE_CLEAR_COLOR|GE_CLEAR_DEPTH);
	sceGuFinish();
	sceGuSync(0,0);

	sceDisplayWaitVblankStart();
	sceGuDisplay(1);

	return 0;
///	return x11_display_init(640, 480, "dacube2", argc, argv);
}

void os_fb_end() {
	if (os_byte_fb == NULL)
		return;
//	free(os_byte_fb);
	os_byte_fb = NULL;

	sceGuTerm();

	sceKernelExitGame();
	return 0;

//	x11_display_close();
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

float curr_ms = 1.0f;
struct timeval time_slices[16];

int os_fb_swap_other_buffer(void* otherbuf) {
#if 0
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
#endif

		if (done != 0) {
			os_fb_end();
			return;
		}

		unsigned int j;
		struct Vertex* vertices;

		sceGuStart(0,list);

		// setup the source buffer as a 512x512 texture, but only copy 480x272
		sceGuTexMode(GE_TPSM_4444,0,0,0);
		sceGuTexImage(0,512,512,512,pixels);
		sceGuTexFunc(GE_TFX_REPLACE,0);
		sceGuTexFilter(GE_FILTER_POINT,GE_FILTER_POINT);
		sceGuTexScale(1.0f/512.0f,1.0f/512.0f); // scale UVs to 0..1
		sceGuTexOffset(0,0);
		sceGuAmbientColor(0xffffffff);

		// do a striped blit (takes the page-cache into account)

		for (j = 0; j < 480; j = j+SLICE_SIZE)
		{
			vertices = (struct Vertex*)sceGuGetMemory(2 * sizeof(struct Vertex));

			vertices[0].u = j; vertices[0].v = 0;
			vertices[0].color = 0;
			vertices[0].x = j; vertices[0].y = 0; vertices[0].z = 0;
			vertices[1].u = j+SLICE_SIZE; vertices[1].v = 272;
			vertices[1].color = 0;
			vertices[1].x = j+SLICE_SIZE; vertices[1].y = 272; vertices[1].z = 0;

			sceGuDrawArray(GU_PRIM_SPRITES,GE_SETREG_VTYPE(GE_TT_16BIT,GE_CT_4444,0,GE_MT_16BIT,0,0,0,0,GE_BM_2D),2,0,vertices);
		}

		sceGuFinish();
		sceGuSync(0,0);

		float curr_fps = 1.0f / curr_ms;

//		sceDisplayWaitVblankStart();
		sceGuSwapBuffers();

		pspDebugScreenSetXY(0,0);
		pspDebugScreenPrintf("%d.%03d",(int)curr_fps,(int)((curr_fps-(int)curr_fps) * 1000.0f));

		// simple frame rate counter

		gettimeofday(&time_slices[val & 15],0);

		val++;

		if (!(val & 15))
		{
			struct timeval last_time = time_slices[0];
			unsigned int i;

			curr_ms = 0;
			for (i = 1; i < 16; ++i)
			{
				struct timeval curr_time = time_slices[i];

				if (last_time.tv_usec > curr_time.tv_usec)
				{
					curr_time.tv_sec++;
					curr_time.tv_usec-=1000000;
				}

				curr_ms += ((curr_time.tv_usec-last_time.tv_usec) + (curr_time.tv_sec-last_time.tv_sec) * 1000000) * (1.0f/1000000.0f);

				last_time = time_slices[i];
			}
			curr_ms /= 15.0f;
		}

	return 0;
}

int os_fb_swap_buffers() {
	return os_fb_swap_other_buffer(os_byte_fb);
}


