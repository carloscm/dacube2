
#include "oldskool.h"

#include "font.h"

#warning put multires support here
#define SCREEN_WIDTH 640

static char fontord[] = " abcdefghijlmnopqrstuvwxyz1234567890,.:!?k/'";

int	fpal[18];

void DrawString(int x, int y, char *fb, char *db, char *str, int size, int align) {
	int	a,b,c,d;
	int	pos,rpos,wpos;
	char	alpha;
	char	*search;
	int	width;
	short 	int *fp;
	int	ll;
	char	*tmp;
	int	sc,sc2;

	fp=(short int *) &FontPos;

	tmp=str;
	ll=0;
	while(*tmp!=0){
		c=0;
		search=&fontord;
		width=6;
		alpha=*tmp;
		while(alpha!=*search && c<=69){
			c++;
			search++;
		}
		if(c!=0 && c<=69){
			c--;
			rpos=fp[c];
			width=fp[c+1]-rpos;
		}
		tmp++;
		ll+=width;
	}

	if(align==0) {
		pos=SCREEN_WIDTH*y+x;
	}

	if(align==1) {
		ll=(SCREEN_WIDTH-(ll*size))/2;
		pos=SCREEN_WIDTH*y+ll;
	}
	if(align==2) {
		ll=SCREEN_WIDTH-(ll*size);
		pos=SCREEN_WIDTH*y+ll;
	}
	if(align==3) {
		ll=0;
		pos=SCREEN_WIDTH*y;
	}

	while(*str!=0){
		width=6;
		alpha=*str;	
		c=0;
		search=&fontord;
		while(alpha!=*search && c<=69){
			c++;
			search++;
		}
		if(c!=0 && c<=69){
			c--;
			rpos=fp[c];
			width=fp[c+1]-rpos;
			for(a=0;a<15;a++){
				wpos=pos+SCREEN_WIDTH*a*size;
				for(b=0;b<width;b++){
					d=fb[rpos];
					if(d!=1){
						for(sc=0;sc<size;sc++){
							for(sc2=0;sc2<size;sc2++){
								db[wpos+sc+sc2*SCREEN_WIDTH]=251+d;
							}
						}
					}
					wpos+=size;
					rpos++;
				}
				rpos+=440-width;
			}
		}
		pos+=width*size;
		str++;
	}
}

void	DrawString2(int	x,int y,char *fb,char *db,char *str,int size,int align)
{
	int	a,b,c,d;
	int	pos,rpos,wpos;
	char	alpha;
	char	*search;
	int	width;
	short 	int *fp;
	int	ll;
	char	*tmp;
	int	sc,sc2;

	fp=(short int *) &FontPos;

	tmp=str;
	ll=0;
	while(*tmp!=0){
		c=0;
		search=&fontord;
		width=6;
		alpha=*tmp;
		while(alpha!=*search && c<=69){
			c++;
			search++;
		}
		if(c!=0 && c<=69){
			c--;
			rpos=fp[c];
			width=fp[c+1]-rpos;
		}
		tmp++;
		ll+=width;
	}

	if(align==0) {
		pos=SCREEN_WIDTH*y+x;
	}

	if(align==1) {
		ll=(SCREEN_WIDTH-(ll*size))/2;
		pos=SCREEN_WIDTH*y+ll;
	}
	if(align==2) {
		ll=SCREEN_WIDTH-(ll*size);
		pos=SCREEN_WIDTH*y+ll;
	}
	if(align==3) {
		ll=0;
		pos=SCREEN_WIDTH*y;
	}

	while(*str!=0){
		width=6;
		alpha=*str;	
		c=0;
		search=&fontord;
		while(alpha!=*search && c<=69){
			c++;
			search++;
		}
		if(c!=0 && c<=69){
			c--;
			rpos=fp[c];
			width=fp[c+1]-rpos;
			for(a=0;a<15;a++){
				wpos=pos+SCREEN_WIDTH*a*size;
				for(b=0;b<width;b++){
					d=fb[rpos];
					if(d!=1){
						for(sc=0;sc<size;sc++){
							for(sc2=0;sc2<size;sc2++){
								db[wpos+sc+sc2*SCREEN_WIDTH]=245+d;
							}
						}
					}
					wpos+=size;
					rpos++;
				}
				rpos+=440-width;
			}
		}
		pos+=width*size;
		str++;
	}
}

void DeCrunchFont(unsigned char *fb) {
	char *f;
	int i, j, c1, c2;
	int pos = 0;

	f=(char *) &CrunchedFont;
	for (i = 251, j = 0; i < 256; ++i, ++j) {
		os_fb_set_palette(i, FontPal[j*3],
			FontPal[j*3 + 1],
			FontPal[j*3 + 2]);
		fpal[j*3  ]=FontPal[j*3];
		fpal[j*3+1]=FontPal[j*3+1];
		fpal[j*3+2]=FontPal[j*3+2];
	}

	for (i = 0; i < 15; ++i) {
		for (j = 0; j < 440/2; ++j) {
			c1 = f[pos];
			c2 = c1>>4;
			c1 = c1&0xf;
			fb[pos*2] = c2;
			fb[pos*2+1] = c1;
			++pos;
		}
	}
}

