
extern int ddraw_display_init (unsigned int width, unsigned int height, char *title);
extern int ddraw_display_close ();
extern int ddraw_display_lock (void** buf, int* bpp, int* pitch);
extern int ddraw_display_set_palette (unsigned char*);
extern int ddraw_display_unlock ();

