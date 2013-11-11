
#define WIN32_LEAN_AND_MEAN
#include <ddraw.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "win.h"
#include "../resources/resource.h"

#define PANIC(__m) MessageBox(NULL,__m,"ccm sucks",MB_OK|MB_ICONSTOP)

#define TRYDD(__call,__msg) do { if (##__call != DD_OK) { PANIC(__msg); return 1; } } while(0)

typedef struct _ddraw_window {
	LPDIRECTDRAW2 dd2;
	LPDIRECTDRAWSURFACE3 surface3;
	LPDIRECTDRAWSURFACE3 backsurface3;
	LPDIRECTDRAWCLIPPER clipper;
	LPDIRECTDRAWPALETTE palette;
	HWND hWnd;
	unsigned int width, height, bpp, fullscreen, skipdisplay;
} ddraw_window;

static ddraw_window* ddraw_current_window = NULL;

static void ddraw_win_update (HWND hWnd) {
	MSG message;
	while (PeekMessage (&message, hWnd, 0, 0, PM_REMOVE)) {
		TranslateMessage (&message);
		DispatchMessage (&message);
	}
}

static void ddraw_paint_window (ddraw_window *win) {
	RECT rcRectSrc;
	RECT rcRectDest;
	POINT p;

	if (win->fullscreen) {
		//IDirectDrawSurface3_Flip(win->surface3, NULL, DDFLIP_WAIT);
		IDirectDrawSurface3_Flip(win->surface3, NULL, 0	);
		return;
	}

	p.x = 0; p.y = 0;
	ClientToScreen (win->hWnd, &p);
        GetClientRect (win->hWnd, &rcRectDest);
        OffsetRect (&rcRectDest, p.x, p.y);
        SetRect (&rcRectSrc, 0, 0, win->width - 1, win->height - 1);
#if 0
	{
		DDBLTFX fx;
		HRESULT r = DD_OK;
		fx.dwSize = sizeof(DDBLTFX);
		fx.dwDDFX = DDBLTFX_NOTEARING;
		r = win->ddSurface->lpVtbl->Blt(win->ddSurface, &rcRectDest, win->ddBackSurface, &rcRectSrc,
			DDBLT_WAIT | DDBLT_DDFX, &fx);
	}
#else
	IDirectDraw2_WaitForVerticalBlank(win->dd2, DDWAITVB_BLOCKBEGIN, NULL);
	if (IDirectDrawSurface3_Blt(win->surface3, &rcRectDest, win->backsurface3, &rcRectSrc,
		0, NULL) == DDERR_SURFACEBUSY) {
		IDirectDrawSurface3_Blt(win->surface3, &rcRectDest, win->backsurface3, &rcRectSrc,
			DDBLT_WAIT, NULL);
	}
#endif
}

static int ddraw_quit_status = 0;

static long CALLBACK ddraw_win_proc (HWND hWnd, UINT message, WPARAM wParam, LPARAM lParam) {
	switch (message) {
		case WM_ACTIVATE:
			if (ddraw_current_window->fullscreen != 1)
				return 0;
			if (wParam == WA_ACTIVE) {
				ShowCursor(FALSE);
			} else {
				ShowCursor(TRUE);
			}
			
		break;  
		case WM_DESTROY:
		case WM_CLOSE:
			ddraw_quit_status = 1;
			return 0;
		break;

		case WM_KEYDOWN:
		if (((int)(wParam)) == VK_ESCAPE) {
			ddraw_quit_status = 1;
			return 0;
		}
		break;

		case WM_SYSCOMMAND:
		if ((wParam & 0xFFF0) == SC_SCREENSAVE ||
			(wParam & 0xFFF0) == SC_MONITORPOWER)
			return 0;
		break;
/*
		case WM_PAINT:
			memset(&PS, 0x00, sizeof(PAINTSTRUCT));
			BeginPaint (hWnd, &PS);

			ddraw_paint_window (ddraw_current_window);

			EndPaint (hWnd, &PS);
			return 0;
		break;
*/
		
	}
	return DefWindowProc (hWnd, message, wParam, lParam);
}

static int ddraw_window_register_class() {
	WNDCLASSEX classWnd;
	HINSTANCE instance = GetModuleHandle(NULL);

	// register window class
	classWnd.cbSize        = sizeof(WNDCLASSEX);
	classWnd.hInstance     = instance;
	classWnd.lpszClassName = "fznintro";
	classWnd.style         = CS_VREDRAW|CS_HREDRAW|CS_OWNDC;
	classWnd.hIcon         = LoadIcon (GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICONSMALL));
	classWnd.hIconSm       = LoadIcon (GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICONSMALL));
	classWnd.lpszMenuName  = 0;
	classWnd.cbClsExtra    = 0;
	classWnd.cbWndExtra    = 0;
	classWnd.hbrBackground = 0;//(HBRUSH) GetStockObject(BLACK_BRUSH);
	classWnd.lpfnWndProc = (WNDPROC) ddraw_win_proc;
	classWnd.hCursor = LoadCursor (0, IDC_ARROW);

	if (RegisterClassEx (&classWnd) == 0) {
		PANIC ("Cannot register new class");
		return 1;
	}

	return 0;
}

static HWND ddraw_create_window(unsigned int width, unsigned int height, char *title) {
	HWND hWnd;
	RECT WindowRect;
	WindowRect.left=(long)0;
	WindowRect.right=(long)width;
	WindowRect.top=(long)0;
	WindowRect.bottom=(long)height;
	
	AdjustWindowRectEx(&WindowRect, WS_OVERLAPPEDWINDOW, FALSE, WS_EX_STATICEDGE);

	hWnd = CreateWindowEx (WS_EX_STATICEDGE,
		"fznintro", title,
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		WindowRect.right-WindowRect.left,
		WindowRect.bottom-WindowRect.top,
		NULL, NULL, GetModuleHandle(NULL), NULL);

	if (!hWnd) {
		PANIC ("Cannot create window");
		return hWnd;
	}
	
	ShowWindow (hWnd, SW_SHOW);
        SetFocus (hWnd);
        SetActiveWindow (hWnd);
        SetForegroundWindow (hWnd);

	return hWnd;
}

static int ddraw_init_directdraw(ddraw_window* win, int level) {
	LPDIRECTDRAW dd = NULL;
	TRYDD(DirectDrawCreate(NULL, &dd, 0), "Cannot create IDirectDraw object");
	TRYDD(IDirectDraw_QueryInterface(dd, &IID_IDirectDraw2, &win->dd2),
		"Cannot create IDirectDraw2 object");
	IDirectDraw_Release(dd);
	TRYDD(IDirectDraw2_SetCooperativeLevel(win->dd2, win->hWnd, level),
		"Cannot set cooperative level");
	return 0;
}

static int ddraw_init_primary(ddraw_window* win, LPDDSURFACEDESC desc) {
	LPDIRECTDRAWSURFACE surface = NULL;
	TRYDD(IDirectDraw2_CreateSurface(win->dd2, desc, &surface, NULL),
		"Cannot create IDirectDrawSurface object for primary surface");
	TRYDD(IDirectDrawSurface_QueryInterface(surface, &IID_IDirectDrawSurface3, &win->surface3),
		"Cannot create IDirectDrawSurface3 object for primary surface");
	IDirectDrawSurface_Release(surface);
	return 0;
}

static int ddraw_display_init_windowed (unsigned int width, unsigned int height, char *title) {
	DDSURFACEDESC desc;
	DDPIXELFORMAT format;
	LPDIRECTDRAWSURFACE backsurface = NULL;
	ddraw_window* win = NULL;

	if (ddraw_current_window != NULL)
		return 1;

	ddraw_current_window = (ddraw_window *)malloc(sizeof(ddraw_window));
	memset(ddraw_current_window, 0, sizeof(ddraw_window));

	win = ddraw_current_window;

	if (ddraw_window_register_class())
		return 1;

	win->hWnd = ddraw_create_window(width, height, title);
	if (!win->hWnd)
		return 1;

	if (ddraw_init_directdraw(win, DDSCL_NORMAL))
		return 1;

	memset(&desc, 0, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_CAPS;
	desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;

	if (ddraw_init_primary(win, &desc))
		return 1;

	TRYDD(IDirectDraw2_CreateClipper(win->dd2, 0, &win->clipper, NULL),
		"Cannot create IDirectDrawClipper object");

	TRYDD(IDirectDrawClipper_SetHWnd(win->clipper, 0, win->hWnd),
		"Cannot attach clipper to window");

	TRYDD(IDirectDrawSurface3_SetClipper(win->surface3, win->clipper),
		"Cannot attach clipper to primary surface");

	memset (&desc, 0, sizeof(desc));
	desc.dwSize = sizeof (desc);
	desc.dwFlags = DDSD_CAPS | DDSD_HEIGHT | DDSD_WIDTH;
	desc.ddsCaps.dwCaps = DDSCAPS_OFFSCREENPLAIN;// | DDSCAPS_SYSTEMMEMORY;
	desc.dwWidth = width;
	desc.dwHeight = height;

	TRYDD(IDirectDraw2_CreateSurface(win->dd2, &desc, &backsurface, NULL),
		"Cannot create IDirectDrawSurface object for backbuffer");
	
	TRYDD(IDirectDrawSurface_QueryInterface(backsurface, &IID_IDirectDrawSurface3,
		&win->backsurface3), "Cannot create IDirectDrawSurface3 object for backbuffer");

	IDirectDrawSurface_Release(backsurface);

	memset(&format, 0, sizeof(format));
	format.dwSize = sizeof(format);

	TRYDD(IDirectDrawSurface3_GetPixelFormat(win->backsurface3, &format),
		"Cannot query backbuffer pixel format");

	if (((format.dwFlags & DDPF_RGB) == 0) || (
                format.dwRGBBitCount != 16 &&
		format.dwRGBBitCount != 24 &&
		format.dwRGBBitCount != 32 ) ) {
		PANIC ("No valid surface pixel format");
		return 1;
	}

	win->fullscreen = 0;
	win->skipdisplay = 0;
	win->width = width;
	win->height = height;
	win->bpp = format.dwRGBBitCount;
	// check for 15bpp
	if (win->bpp == 16 && format.dwGBitMask == 0x3e0) {
		win->bpp = 15;
	}

	return 0;
}

int ddraw_display_set_palette (unsigned char* pal) {
	PALETTEENTRY pal2[256];
	int i, j;
	if (ddraw_current_window == NULL)
		return 1;
	if (ddraw_current_window->palette == NULL)
		return 1;
	for (i = 0, j = 0; i < 256; ++i, j+=4) {
		pal2[i].peRed = (pal[j])<<2;
		pal2[i].peGreen = (pal[j+1])<<2;
		pal2[i].peBlue = (pal[j+2])<<2;
		pal2[i].peFlags = PC_NOCOLLAPSE;
	}
	TRYDD(IDirectDrawPalette_SetEntries(ddraw_current_window->palette, 0, 0, 256, pal2),
		"Cannot set palette");
	return 0;
}

static int ddraw_display_init_fullscreen(unsigned int width, unsigned int height, char* title) {
	DDSURFACEDESC desc;
	DDPIXELFORMAT format;
	DDSCAPS caps;
	ddraw_window* win = NULL;
	int bpps[4] = {
		8, 32, 16, 24
	};
	int i;

	if (ddraw_current_window != NULL)
		return 1;

	ddraw_current_window = (ddraw_window *)malloc(sizeof(ddraw_window));
	memset(ddraw_current_window, 0, sizeof(ddraw_window));

	win = ddraw_current_window;

	if (ddraw_window_register_class())
		return 1;

	win->hWnd = ddraw_create_window(width, height, title);
	if (!win->hWnd)
		return 1;

	if (ddraw_init_directdraw(win, DDSCL_EXCLUSIVE | DDSCL_FULLSCREEN))
		return 1;

	ShowCursor(FALSE);
	
	for (i = 0; i < 4; ++i) {
		if (IDirectDraw2_SetDisplayMode(win->dd2, width, height, bpps[i], 0, 0) == DD_OK)
			break;
	}
	if (i == 4) {
		PANIC("Cannot set display mode");
		return 1;
	}

	memset(&desc, 0, sizeof(desc));
	desc.dwSize = sizeof(desc);
	desc.dwFlags = DDSD_CAPS | DDSD_BACKBUFFERCOUNT;
	desc.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE | DDSCAPS_FLIP | DDSCAPS_COMPLEX;
        desc.dwBackBufferCount = 1;

	if (ddraw_init_primary(win, &desc))
		return 1;

	memset(&caps, 0, sizeof(caps));
	caps.dwCaps = DDSCAPS_BACKBUFFER;
	TRYDD(IDirectDrawSurface3_GetAttachedSurface(win->surface3, &caps, &win->backsurface3),
		"Cannot get attached back surface");

	memset(&format, 0, sizeof(format));
	format.dwSize = sizeof(format);

	TRYDD(IDirectDrawSurface3_GetPixelFormat(win->backsurface3, &format),
		"Cannot query back surface pixel format");

	if (format.dwFlags & DDPF_RGB) {
		if (format.dwRGBBitCount != 16 &&
			format.dwRGBBitCount != 24 &&
			format.dwRGBBitCount != 32 &&
			format.dwRGBBitCount != 8) {
			PANIC ("No valid surface pixel format");
			return 1;
		}
		if (format.dwRGBBitCount == 8 &&
			((format.dwFlags & DDPF_PALETTEINDEXED8) == 0)) {
			PANIC ("No valid 8bpp surface pixel format");
			return 1;
		}
	}

	win->fullscreen = 1;
	win->skipdisplay = 0;
	win->width = width;
	win->height = height;
	win->bpp = format.dwFlags & DDPF_PALETTEINDEXED8 ?
		8 : format.dwRGBBitCount;
	// check for 15bpp
	if (win->bpp == 16 && format.dwGBitMask == 0x3e0) {
		win->bpp = 15;
	}

	if (win->bpp == 8) {
		int i;
		PALETTEENTRY pal2[256];

		for (i = 0; i < 256; ++i) {
			pal2[i].peRed = 0;
			pal2[i].peGreen = 0;
			pal2[i].peBlue = 0;
			pal2[i].peFlags = PC_NOCOLLAPSE;
		}

		TRYDD(IDirectDraw2_CreatePalette(win->dd2, DDPCAPS_8BIT | DDPCAPS_ALLOW256, pal2,
				&win->palette, NULL), "Cannot create palette");

		//TRYDD(IDirectDrawSurface3_SetPalette(win->backsurface3, win->palette),
			//"Cannot set palette on back surface");

		TRYDD(IDirectDrawSurface3_SetPalette(win->surface3, win->palette),
			"Cannot set palette on primary surface");
	}

	return 0;
}

#define C_QUIT 0
#define C_WIN_OLD 1
#define C_WIN_NEW 2
#define C_FULL_OLD 3
#define C_FULL_NEW 4

static BOOL CALLBACK options_dialog_callback(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam) {
	switch (uMsg) {
		case WM_INITDIALOG: {
				RECT rc;
				GetWindowRect(hwndDlg, &rc); 
				SetWindowPos(hwndDlg, NULL, 
					((GetSystemMetrics(SM_CXSCREEN) - (rc.right - rc.left)) / 2), 
					((GetSystemMetrics(SM_CYSCREEN) - (rc.bottom - rc.top)) / 2), 
					0, 0, SWP_NOSIZE | SWP_NOACTIVATE);
				SetClassLong(hwndDlg, GCL_HICON,
					LoadIcon (GetModuleHandle(NULL), MAKEINTRESOURCE(IDI_ICONSMALL)));
				return TRUE;
			}
		break;
		case WM_COMMAND:
	      		switch (LOWORD(wParam)) {
				case IDQUITZ:
					EndDialog(hwndDlg, C_QUIT);
				break;
				case IDSTART: {
					int old = 0, win = 0, r = C_QUIT;
					if (IsDlgButtonChecked(hwndDlg, IDC_CHECKFUZ) == BST_CHECKED) {
						old = 1;
					}
					if (IsDlgButtonChecked(hwndDlg, IDC_CHECKWIN) == BST_CHECKED) {
						win = 1;
					}
					if (win == 0 && old == 0) {
						r = C_FULL_NEW;
					}
					if (win == 0 && old == 1) {
						r = C_FULL_OLD;
					}
					if (win == 1 && old == 0) {
						r = C_WIN_NEW;
					}
					if (win == 1 && old == 1) {
						r = C_WIN_OLD;
					}
				  	EndDialog(hwndDlg, r);
				} break;
			}
		break;
		case WM_CTLCOLORSTATIC:
		case WM_CTLCOLORBTN:
			SetBkColor((HDC)wParam, RGB(255, 255, 255));
			return GetStockObject(WHITE_BRUSH);
		break;
		case WM_CLOSE:
		case WM_DESTROY:
			EndDialog(hwndDlg, -1);
		break;
	}
	return FALSE;

}

int ddraw_display_init (unsigned int width, unsigned int height, char *title) {
	int doit = DialogBox(GetModuleHandle(NULL), (LPCTSTR) IDD_DIALOG1, NULL, options_dialog_callback);
	int r = -1;
	int init_r = -1;
	switch (doit) {
		case C_FULL_NEW:
		case C_FULL_OLD:
			init_r = ddraw_display_init_fullscreen(width, height, title);
		break;
		case C_WIN_NEW:
		case C_WIN_OLD:
			init_r = ddraw_display_init_windowed(width, height, title);
		break;
		default:
			init_r = -1;
		break;
	}
	if (init_r == 0) { // init ok
                if (doit == C_FULL_NEW || doit == C_WIN_NEW) {
			r = 0;
		} else {
			r = 1;
		}
	}
	return r;
}


int ddraw_display_close () {
	ddraw_window *win = ddraw_current_window;
	if (win == NULL)
		return 1;
	if (win->clipper != NULL) {
		IDirectDrawSurface3_SetClipper(win->surface3, NULL);
		IDirectDrawClipper_Release(win->clipper);
	}
	if (win->backsurface3 != NULL && win->fullscreen == 0)
		IDirectDrawSurface3_Release(win->backsurface3);
	if (win->surface3 != NULL)
		IDirectDrawSurface3_Release(win->surface3);
	if (win->palette != NULL)
		IDirectDrawPalette_Release (win->palette);
	if (win->dd2 != NULL) {
		if (win->fullscreen)
			ShowCursor(TRUE);
		IDirectDraw2_Release(win->dd2);
	}
	
	ddraw_win_update (ddraw_current_window->hWnd);
	
	if (ddraw_current_window->hWnd != 0)
		DestroyWindow (ddraw_current_window->hWnd);
	if (ddraw_current_window->hWnd != 0)
		UnregisterClass ("fznintro", GetModuleHandle(NULL));

	free(ddraw_current_window);
	ddraw_current_window = NULL;

	return 0;
}


int ddraw_display_lock (void** buf, int* bpp, int* pitch) {
	ddraw_window *win = ddraw_current_window;
	DDSURFACEDESC lock_desc;

	if (win == NULL)
		return 1;

	if (IDirectDrawSurface3_IsLost(win->surface3) != DD_OK) {
		if (IDirectDrawSurface3_Restore(win->surface3) != DD_OK) {
			win->skipdisplay = 1;
			return 0;
		}
		/* ccm - hay que restaurar la paleta tb... */
	}
	/*if (IDirectDrawSurface3_IsLost(win->backsurface3) != DD_OK) {
		if (IDirectDrawSurface3_Restore(win->backsurface3) != DD_OK) {
			win->skipdisplay = 1;
			return 0;
		}
	} commented per dx5 samples */

	memset(&lock_desc, 0, sizeof(lock_desc));
	lock_desc.dwSize = sizeof(lock_desc);
	if (IDirectDrawSurface3_Lock(win->backsurface3, NULL, &lock_desc,
		DDLOCK_SURFACEMEMORYPTR | DDLOCK_WAIT, NULL) != DD_OK) {
		PANIC ("Cannot lock back surface");
	}

	*buf = lock_desc.lpSurface;
	*bpp = win->bpp;
	*pitch = lock_desc.lPitch;
	
	return 0;
}

int ddraw_display_unlock () {
	if (ddraw_current_window->skipdisplay == 1) {
		ddraw_current_window->skipdisplay = 0;
		return  ddraw_quit_status;
	}

	IDirectDrawSurface3_Unlock(ddraw_current_window->backsurface3, NULL);

	ddraw_paint_window (ddraw_current_window);
	
	ddraw_win_update(ddraw_current_window->hWnd);

	return ddraw_quit_status;
}

