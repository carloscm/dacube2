#ifndef DRAWCHAR_H
#define DRAWCHAR_H
extern void DrawString(int x, int y, char* fb, char* db, char* str,
	int size, int align);
extern void DrawString2(int x, int y, char* fb, char* db, char* str,
	int size, int align);
extern void DeCrunchFont(unsigned char *fb);

#warning THIS IS FUGLY
extern int fpal[18];

#endif

