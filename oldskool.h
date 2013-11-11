
/* oldskool utility crap to port old crappy code :) consisting of:
 * - 640x480x8bpp frame buffer simulation with virtual palette
 * - arbitrary HZ tick counter
 * - module player wrapper
 * - ccm in pocafeina mode
 */

#ifndef OLDSKOOL_H
#define OLDSKOOL_H

// happy hack
#ifdef __PSP__
extern float nopf(float f);
#undef sinf
#define sinf nopf
#undef cosf
#define cosf nopf
#undef sqrt
#define sqrt nopf
#endif

typedef unsigned char os_byte_t;

/* something gone bad, so alert the user */
/* NOTE: all the os_XXX api alerts the user, there is no need to call os_panic
   if some os_XXX function returns error */
extern void os_panic(char *msg);

/* init tick counter timer */
extern int os_ticks_init(int hz);

/* reset tick counter to 0 */
extern void os_ticks_reset();

/* get tick count */
extern int os_ticks_get();

/* free tick counter */
extern int os_ticks_end();

/* start module playing, -1 on error */
extern int os_mod_start(int argc, char** argv);

/* end module playing */
extern void os_mod_end();

extern int os_mod_get_order();

extern int os_mod_get_row();

/* init buffers and videomode */
/* -1: error
    0: newskool
    1: oldskool
 */
extern int os_fb_init(int argc, char** argv);

/* free buffers and restore videomode */
extern void os_fb_end();

/* get pointer to the 8bpp virtual framebuffer */
extern os_byte_t* os_fb_get_buffer();

/* set virtual palette entry. uses vga component range (0-63) */
extern void os_fb_set_palette(int entry, int red, int green, int blue);

/* get virtual palette entry. returns vga component range (0-63) */
extern int os_fb_get_palette_red(int entry);
extern int os_fb_get_palette_green(int entry);
extern int os_fb_get_palette_blue(int entry);

/* fill 8bpp framebuffer with a color */
extern void os_fb_clear(int entry);

/* fill 8bpp framebuffer with a color, including crapbuffer :) */
extern void os_fb_clear_full(int entry);

/* convert 8bpp buffer to the real framebuffer and swap real buffers */
/* returns 1 if the user wants to exit */
extern int os_fb_swap_buffers();

/* convert 8bpp buffer to the real framebuffer and swap real buffers */
/* returns 1 if the user wants to exit */
extern int os_fb_swap_other_buffer(void* buffer);

#endif

