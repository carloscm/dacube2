
#ifndef X11_H
#define X11_H

extern int x11_display_init (unsigned int w, unsigned int h, char *name, int argc, char** argv);
extern int x11_display_close ();

extern int x11_get_bpp();
extern char* x11_get_buffer();
extern int x11_display_swap();

#endif

