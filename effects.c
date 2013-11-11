
int	start_txt;

#include <stdlib.h>
#include <string.h>
#include <math.h>

#include "oldskool.h"
#ifdef __COMPILE_MINIFMOD__
#undef __COMPILE_MINIFMOD__
#endif
#include "minifmod/minifmod.h"

#include "struct.h"
#include "3dengine.h"
#include "title.h"
#include "drawchar.h"

#define	MAXCUBES 90
#define SUI_RAND_MAX 32768
#define PI 3.141592654

#warning put multires support here
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480
#define SCREEN_SIZE (640*480)

#warning SetOrder?? WTF??
void __sux(int n) {
}
#define USMP_SetOrder(__o) __sux(__o)

#define Order (os_mod_get_order())
#define Row (os_mod_get_row())

#ifndef WIN32
#define fsin sinf
#define fcos cosf
#else
#define fsin sin
#define fcos cos
#endif
#define ffabs fabs
#define fsqrt sqrt

int suiRand();
float disp (float v, float r);
static int oldseed = 6;
extern char *FontBuffer;
extern int oldskool;

char	*Cred1="   bp:";
char	*Cred1_="code.lyrics   ";
char	*Cred2="   trace:";
char	*Cred2_="gfx.dezign   ";
char	*Cred3="   sml:";
char	*Cred3_="muzik   ";

char	*_SVG_oldskool="savage";
char	*_SVG_newskool="fuzzion";
#define svg (oldskool ? _SVG_oldskool : _SVG_newskool)

char	*_LASTM_oldskool=": da cube 2 :: savage'99 :";
char	*_LASTM_newskool=": da cube 2 :: fuzzion'99 :";
#define LastM (oldskool ? _LASTM_oldskool : _LASTM_newskool)

CAMERA	View;

int	radius[5]={300,250,200,150,100};
int	color2[5]={50,100,150,200,250};	//250,200,150,100,50};	//45,35,25,15,0};
int	color[5]={45,35,25,15,0};
int	posy[6]={0,40,80,120,160,400};
int	vpos[4*6];
VECTOR	CubOrg[10*6];
int	paleta[768];
int	pal[18];

//int	fstable[64];
int	auxpal[768];

int	mes=0;

int 	timer_start;

char	*Messages[]={
"i'm a cube",
"you're a cube",
"everyone is a cube",
"we're like pets",
"caged in a cubic society",
"so closed...",
"claustrophobia is making us blind",
"there's no light",
"no exit",
"we only see the walls of a cube",
"filled with darkness",
"for those we'll never know",
"just as they want",
"they destroyed every try",
"we have made",
"to break the wall",
"and they erased and manipulated",
"our minds",
"to the point of selfdestruction",
"time is passing by",
"and instead of fighting",
"we create more walls...",
"walls of despair",
"where our sons will cry forever",
"and forget the truth",
"swimming in a cloud of gloom"
"they'll find a way to break it all",
"but those who meet the light",
"won't return to save the others",
"we're like working ants",
"easy to crush",
"no stop",
"no rebellion",
"and when we die",
"nobody will remember us",
"there's no way",
"we're damned",
"following ideologies",
"created by ourselves",
"to destroy us",
"and everything we love",
"we're too deep",
"to reach the surface",
"and breathe no toxic air",
"we're used to their unyielding will",
"and we'll never change this",
"we must be blind",
"we must be blind",
" ",
};	//54

char *_GREETS_oldskool[]=
{"mcd","dsk!","inside","anaconda","wild bits","cuc makers","noice",
"incognita","tdr","wavemasters","movedata","centosis",
"ozone","genesis","eklipse","network","hansa"," "," "}; 
char *_GREETS_newskool[]=
{"rgba", "concept", "anaconda", "stravaganza", "iguana", "threepixels",
"collapse", "unknown", "escena.org", "talsit", "zonan", "tlotb",
"ozone", "network", "hansa", "dlirium", "scenesp.org", " ", " "};
#define Greets (oldskool ? _GREETS_oldskool : _GREETS_newskool)

char *_FRASE_oldskool="dedicated to chc/mcd ";
char *_FRASE_newskool="2003 final version ";
#define frase (oldskool ? _FRASE_oldskool : _FRASE_newskool)

char *messages2[]=
{
"procreate cubes",
"feed cubes",
"make cubes",
"destroy spheres",
"save cubes",
"from them",
"kill soft edges",
"raze those ugly torus",
"break cylinders",
"no beziers",
"no splines",
"stop raytracers",
"let's gonna make",
"this world",
"a better place...",
"a cubic place!!!",
"with no colors",
"so we can't see",
"the future",
"we'll always be",
"closed inside",
"they closed our eyes",
"they disabled our senses",
"they created our world",
"they have stolen our life",
"forever..."};

void clsi(int *where, int n, int value) {
	memset(where, value, n*4);
}

void Cls(char *where, int n) {
	memset(where, 0, n*4);
}

void ClsF(char *where, int n) {
	memset(where, 0x7fffffff, n*4);
}

void move32(char *from, char* to, int n) {
	memmove(to, from, n*4);
}

void Cls2(int *where, int n, int value) {
	int low = value & 0xff;
	memset(where, low | (low << 8) | (low << 16) | (low << 24), n*4);
}

void	Cubes4(unsigned char *dst)
{
	VECTOR	av;
	int	a,b,c,fs;
	float	angle;	//,r;
	int	f1,f2;	//,f3,f4,f5,f6;

	InitEngine();
        USMP_SetOrder(0x0A);

	View.Position.v1=-150.0;
	View.Position.v2=0.0;
	View.Position.v3=-2000.0;
	View.Rotation.v1=0.0;
	View.Rotation.v2=30.0;
	View.Rotation.v3=0.0;
	View.FOV=512.0;
	View.Roll=0.0;

	av.v1=0.0;
	av.v2=0.0;
	av.v3=900.0;
	CreateCubeWrel(av,250,250);

	av.v1=0.0;
	av.v2=500.0;
	av.v3=900.0;
	CreateCubeWrel(av,175,250);

	av.v1=0.0;
	av.v2=-500.0;
	av.v3=900.0;
	CreateCubeWrel(av,175,250);

	av.v1=-500.0;
	av.v2=0.0;
	av.v3=900.0;
	CreateCubeWrel(av,175,250);


	av.v1=500.0;
	av.v2=0.0;
	av.v3=900.0;
	CreateCubeWrel(av,175,250);

	av.v1=0.0;
	av.v2=900.0;
	av.v3=900.0;
	CreateCubeWrel(av,125,250);

	av.v1=0.0;
	av.v2=-900.0;
	av.v3=900.0;
	CreateCubeWrel(av,125,250);


	av.v1=-900.0;
	av.v2=0.0;
	av.v3=900.0;
	CreateCubeWrel(av,125,250);

	av.v1=900.0;
	av.v2=0.0;
	av.v3=900.0;
	CreateCubeWrel(av,125,250);

	av.v1=-1150.0;
	av.v2=0.0;
	av.v3=900.0;

	CreateCubeWrel(av,50,250);

	av.v1=1150.0;
	av.v2=0.0;
	av.v3=900.0;

	CreateCubeWrel(av,50,250);

	av.v1=0.0;
	av.v2=-1150.0;
	av.v3=900.0;

	CreateCubeWrel(av,50,250);

	av.v1=0.0;
	av.v2=1150.0;
	av.v3=900.0;

	CreateCubeWrel(av,50,250);


	for(a=0;a<13;a++){
		Rot[a].v1=0.0;
		Rot[a].v2=0.0;
		Rot[a].v3=0.0;
	}

	LoadMov();

	angle=0.0;
	timer_start=os_ticks_get();
	while(Order<0x0B){
		Cls2(dst,(640*480)>>2,55);	//70);
		c=0;	//c=21;

		Rot[0].v3=180.0*fsin(angle);
		for(a=1;a<4;a++){
			Rot[a].v3=180.0*fsin(angle-0.06);
		}

		for(a=4;a<9;a++){
			Rot[a].v3=180.0*fsin(angle-0.12);
		}
		for(a=9;a<13;a++){
			Rot[a].v3=180.0*fsin(angle-0.18);
		}

		if((Order==0xA && (Row==0x00 || Row==0x03 || Row==0x06 || 
				  Row==0x09 || Row==0x10 || Row==0x13 ||
				  Row==0x16 || Row==0x19 || Row==0x1D ||
				  Row==0x20 || Row==0x23 || Row==0x26 ||
				  Row==0x29 || Row==0x30 || Row==0x33 ||
				  Row==0x38 || Row==0x3B || Row==0x3E)) )
/*
		   (Order==0xB && (Row==0x00 || Row==0x02 || Row==0x1E ||
				  Row==0x20 || Row==0x22 || Row==0x30 || Row==0x3E)))
*/
					timer_start-=13;
		UpdateRot();
		Render(&View,dst);

		fs=((int)((os_ticks_get()-start_txt)/98))%53;
//		DrawString(10,460,FontBuffer,dst,Messages[fs],1,1);
		DrawString(10,(0.479*SCREEN_HEIGHT),FontBuffer,dst,Messages[fs],2,2);
				//230

		if (os_fb_swap_buffers())
			dacube2_exit(0);
		angle=0.008*(os_ticks_get()-timer_start);
	}
}

float disp (float v, float r)
{
	float val=0;

	val=suiRand();
	val/=(float) SUI_RAND_MAX;
	val*=(suiRand() < SUI_RAND_MAX/2) ? -1.0 : 1.0;
	return (v+r*val);
}

int suiRand ()
{
	int a;
	a = oldseed * 0x3ec1f;
	a += 7;
	oldseed = a;
	a = a >> 16;
	a &= 0x7fff;
	return a;
}

void	Cubes6(unsigned char *dst)
{
	VECTOR	av;
	int	a,b,c;
	float	angle,r,tmp1,tmp2,angle2;
	int	numlines;
	int	ylines[5];
	int	size[5];
	float	angles[5];
	float	iangles[5];

	InitEngine();


	View.Position.v1=0.0;
	View.Position.v2=0.0;
	View.Position.v3=-1000.0;
	View.Rotation.v1=0.0;
	View.Rotation.v2=0.0;
	View.Rotation.v3=0.0;
	View.FOV=128.0;
	View.Roll=0.0;

	oldseed=0x7867;
				//0x1999;	//pse 2.0
				//0x7867;	//pse
	numlines=0;
	for(a=0;a<40;a++){
		av.v1=disp(0,600);
		av.v2=disp(0,400);
		av.v3=disp(0,700);
		r=disp(100,75);

		CreateCubeWPrimperan(av,r,0);
	}

	for(a=0;a<40;a++){
		Rot[a].v1=0.0;
		Rot[a].v2=0.0;
		Rot[a].v3=0.0;
	}

	oldseed=0x4321;

	timer_start=os_ticks_get();
	angle=0.0;
	while(Order<0x6){
		Cls2(dst,(640*480)>>2,55);	//70);
		if(angle<360) angle=angle+360;
		if(angle>360) angle=angle-360;
		if(angle<0) angle2=360-angle; else angle2=angle;

		View.Rotation.v1=angle2;
		View.Rotation.v2=angle2;	//180*angle/PI;
		View.Rotation.v3=0.0;

		UpdateRot();
		Render(&View,dst);

		for(a=0;a<numlines;a++){
			for(c=0;c<size[a];c++){
				angles[a]=0.02*(os_ticks_get()-timer_start)+iangles[a];
				ylines[a]=(SCREEN_WIDTH*0.0677*fsin(angles[a]))+(0.854*SCREEN_HEIGHT);
					//32.5			//410
				for(b=0;b<SCREEN_WIDTH;b++){
					dst[(ylines[a]+c)*SCREEN_WIDTH+b]=0;
				}
			}
		}
		if((Order==0x2 && Row==0x14 && numlines==0)) {
			size[numlines]=(suiRand()%3)+2;
			iangles[numlines]=disp(0.0,1.57);
			numlines++;
		}
		if((Order==0x2 && Row==0x23 && numlines<2)) {
			size[numlines]=(suiRand()%3)+2;
			iangles[numlines]=disp(0.0,1.57);
			numlines++;
		}
		if((Order==0x3 && Row==0x01 && numlines<3)) {
			size[numlines]=(suiRand()%3)+2;
			iangles[numlines]=disp(0.0,1.57);
			numlines++;
		}
		if((Order==0x3 && Row==0x14 && numlines<4)) {
			size[numlines]=(suiRand()%3)+2;
			iangles[numlines]=disp(0.0,1.57);
			numlines++;
		}
		if((Order==0x3 && Row==0x23 && numlines<5)) {
			size[numlines]=(suiRand()%3)+2;
			iangles[numlines]=disp(0.0,1.57);
			numlines++;
		}

		if((Order>=0x2 && Order!=0x5) || 
		   (Order==0x5 && (Row<0x1E || Row>0x1F))) 
                      DrawString(10,(0.729*SCREEN_HEIGHT),FontBuffer,dst,svg,7,1);
				//350
		if((Order==0x4 && (Row<0x1E || Row>0x1F)) || Order>0x4){
                      DrawString(10,(0.156*SCREEN_HEIGHT),FontBuffer,dst,Cred1,3,3);
                      DrawString(10,(0.156*SCREEN_HEIGHT),FontBuffer,dst,Cred1_,3,2);
				//75
		}

		if((Order==0x4 && Row>0x1F) || Order>0x4){
                      DrawString(10,(0.281*SCREEN_HEIGHT),FontBuffer,dst,Cred2,3,3);
                      DrawString(10,(0.281*SCREEN_HEIGHT),FontBuffer,dst,Cred2_,3,2);
				//135
		}

		if(Order>=0x5){
                      DrawString(10,(0.406*SCREEN_HEIGHT),FontBuffer,dst,Cred3,3,3);
                      DrawString(10,(0.406*SCREEN_HEIGHT),FontBuffer,dst,Cred3_,3,2);
		}

		if (os_fb_swap_buffers())
			dacube2_exit(0);
		angle=-0.05*(os_ticks_get()-timer_start);
	}
}

void title2(unsigned char *dst)
{
#warning title2() uses custom buffers with Flip()

	int	a,b,pos,pos2;
	unsigned char c1,c2;	//,c3,c4;
	char	*t1;
	char	*t2;
	char	*tp1;
	char	*tp2;
	char	*WorkBuffer;
	VECTOR	av;
	float	angle;
	int	ro;
	int	ffx,ffy,pr,pry,prx;
	int	invffx,invffy;

	USMP_SetOrder(0x6);

	View.Position.v1=0.0;
	View.Position.v2=0.0;
	View.Position.v3=-1000.0;
	View.Rotation.v1=0.0;
	View.Rotation.v2=0.0;
	View.Rotation.v3=0.0;
	View.FOV=512.0;
	View.Roll=0.0;

	WorkBuffer=(char *) malloc(640*480);

	InitEngine();
	Cls2(WorkBuffer,(640*480)>>2,250);
	//Flip(WorkBuffer);
	os_fb_swap_other_buffer(WorkBuffer);

	ro=Row;
	while(ro==Row){
		//Flip(WorkBuffer);
		//os_fb_swap_other_buffer(WorkBuffer);
	}
/*
	clsi(&paleta,768*4,63);

	for(a=0;a<256;a++){
		outp(0x3c8,a);
		outp(0x3c9,paleta[a*3]);
		outp(0x3c9,paleta[a*3+1]);
		outp(0x3c9,paleta[a*3+2]);
	}
*/
	av.v1=0.0;
	av.v2=0.0;
	av.v3=-650.0;
	CreateCubeTriFlat(av,100,250);


	t1=(char *) &dacube1;
	t2=(char *) &dacube2;
	tp1=(char *) &cubepal1;
	tp2=(char *) &cubepal2;

	for(a=0;a<6;a++){
		pal[a*3  ]=tp1[a*3  ];
		pal[a*3+1]=tp1[a*3+1];
		pal[a*3+2]=tp1[a*3+2];
	}
	for(a=0;a<6;a++){
		os_fb_set_palette(a, pal[a*3], pal[a*3+1], pal[a*3+2]);
	}

	Cls2(WorkBuffer,(640*480)>>2,0);
	pos=0;
	pos2=(479-151)*640;
	for(a=240;a>0;a--){
		for(b=0;b<640/2;b++){
			c1=t1[pos];
			c2=c1>>4;
			c1=c1&0xf;
			WorkBuffer[pos2  ]=c2;
			WorkBuffer[pos2+1]=c1;
			pos++;
			pos2+=2;
		}
		pos2-=1280;
	}
//	ffy=(65536*SCREEN_HEIGHT)/480;
//	ffx=(65536*SCREEN_WIDTH)/640;
	invffx=(65536*640)/SCREEN_WIDTH;
	invffy=(65536*480)/SCREEN_HEIGHT;
	pr=0;	pry=0;
	for(a=0;a<SCREEN_HEIGHT;a++){	
		prx=0;
		for(b=0;b<SCREEN_WIDTH;b++){
			dst[pr]=WorkBuffer[(pry>>16)*SCREEN_WIDTH+(prx>>16)];
			pr++;
			prx+=invffx;
		}
		pry+=invffy;
	}
	move32(dst,WorkBuffer,(SCREEN_SIZE)>>2);

	LoadMov();
	angle=0.0;
	timer_start=os_ticks_get();
	while(Order<0x7){
		move32(WorkBuffer,dst,(SCREEN_SIZE)>>2);
		Rot[0].v1=angle*2;
		Rot[0].v2=angle;
		Rot[0].v3=angle;
		Mov[0].v3=-650+5.5*(os_ticks_get()-timer_start);
		UpdateMov();
		UpdateRot();
		Render(&View,dst);
		if (os_fb_swap_buffers())
			dacube2_exit(0);
		angle=0.5*(os_ticks_get()-timer_start);
	}

	Cls2(WorkBuffer,(640*480)>>2,250);
	//Flip(WorkBuffer);
	os_fb_swap_other_buffer(WorkBuffer);
	ro=Row;
	while(ro==Row){
		//Flip(WorkBuffer);
	}

	pos=0;
	pos2=479*640;
	for(a=480;a>0;a--){
		for(b=0;b<640/2;b++){
			c1=t2[pos];
			c2=c1>>4;
			c1=c1&0xf;
			WorkBuffer[pos2  ]=c2;
			WorkBuffer[pos2+1]=c1;
			pos++;
			pos2+=2;
		}
		pos2-=1280;
	}

//	ffy=(65536*SCREEN_HEIGHT)/480;
//	ffx=(65536*SCREEN_WIDTH)/640;
	invffx=(65536*640)/SCREEN_WIDTH;
	invffy=(65536*480)/SCREEN_HEIGHT;

	pr=0;	pry=0;
	for(a=0;a<SCREEN_HEIGHT;a++){	
		prx=0;
		for(b=0;b<SCREEN_WIDTH;b++){
			dst[pr]=WorkBuffer[(pry>>16)*SCREEN_WIDTH+(prx>>16)];
			pr++;
			prx+=invffx;
		}
		pry+=invffy;
	}

	for(a=0;a<9;a++){
		os_fb_set_palette(a, tp2[a*3  ], tp2[a*3+1], tp2[a*3+2]);
	}
	while(Order<0x8){
		if (os_fb_swap_buffers())
			dacube2_exit(0);
	}
	free(WorkBuffer);

}

void title2_2(unsigned char *dst)
{
#warning title2_2() uses custom buffers with Flip()
	int	a,b,pos,pos2;
	unsigned char c1,c2;	//,c3,c4;
	char	*t1;
	char	*t2;
	char	*tp1;
	char	*tp2;
	char	*WorkBuffer;
	VECTOR	av;
	float	angle;
	int	ro;
	int	ffx,ffy,pr;

	View.Position.v1=0.0;
	View.Position.v2=0.0;
	View.Position.v3=-1000.0;
	View.Rotation.v1=0.0;
	View.Rotation.v2=0.0;
	View.Rotation.v3=0.0;
	View.FOV=512.0;
	View.Roll=0.0;

	WorkBuffer=(char *) malloc(640*480);

	InitEngine();
	Cls2(WorkBuffer,(640*480)>>2,250);
	//Flip(WorkBuffer);
	os_fb_swap_other_buffer(WorkBuffer);


	ro=Row;
	while(ro==Row){
		//Flip(WorkBuffer);
	}

	t1=(char *) &dacube1;
	t2=(char *) &dacube2;
	tp1=(char *) &cubepal1;
	tp2=(char *) &cubepal2;

	for(a=0;a<6;a++){
		pal[a*3  ]=tp1[a*3  ];
		pal[a*3+1]=tp1[a*3+1];
		pal[a*3+2]=tp1[a*3+2];
	}
	for(a=0;a<6;a++){
		os_fb_set_palette(a, pal[a*3], pal[a*3+1], pal[a*3+2]);
	}

	Cls2(WorkBuffer,(640*480)>>2,0);

	pos=120*640/2;
	pos2=(479-151-120)*640;

	for(a=120;a>0;a--){	//240
		for(b=0;b<640/2;b++){
			c1=t1[pos];
			c2=c1>>4;
			c1=c1&0xf;
			WorkBuffer[pos2  ]=c2;
			WorkBuffer[pos2+1]=c1;
			pos++;
			pos2+=2;
		}
		pos2-=1280;
	}
	Cls(dst,SCREEN_SIZE>>2);
	//move32(WorkBuffer,dst,(640*190)>>2);
	ffy=(65536*SCREEN_HEIGHT)/480;
	ffx=(65536*SCREEN_WIDTH)/640;
	pr=0;
	for(a=0;a<(190*ffy);a+=ffy){
		for(b=0;b<(640*ffx);b+=ffx){
			dst[pr]=WorkBuffer[(a>>16)*SCREEN_WIDTH+(b>>16)];
			pr++;
		}
	}
	while(Row<0x36){
		if (os_fb_swap_buffers())
			dacube2_exit(0);
	}
	pos=0;
	pos2=(479-151)*640;


	for(a=240;a>120;a--){	//240
		for(b=0;b<640/2;b++){
			c1=t1[pos];
			c2=c1>>4;
			c1=c1&0xf;
			WorkBuffer[pos2  ]=c2;
			WorkBuffer[pos2+1]=c1;
			pos++;
			pos2+=2;
		}
		pos2-=1280;
	}

	pr=0;
	for(a=0;a<(480*ffy);a+=ffy){
		for(b=0;b<(640*ffx);b+=ffx){
			dst[pr]=WorkBuffer[(a>>16)*SCREEN_WIDTH+(b>>16)];
			pr++;
		}
	}

	while(Row<0x3B){
		if (os_fb_swap_buffers())
			dacube2_exit(0);
	}

	pos=0;
	pos2=479*640;
	for(a=480;a>0;a--){
		for(b=0;b<640/2;b++){
			c1=t2[pos];
			c2=c1>>4;
			c1=c1&0xf;
			dst[pos2  ]=c2;
			dst[pos2+1]=c1;
			pos++;
			pos2+=2;
		}
		pos2-=1280;
	}

	for(a=0;a<9;a++){
		os_fb_set_palette(a, tp2[a*3], tp2[a*3+1], tp2[a*3+2]);
	}
	while(Order<0x1A){
		if (os_fb_swap_buffers())
			dacube2_exit(0);
	}
	free(WorkBuffer);
}

void	Cubes7(unsigned char *dst,int part)
{
	VECTOR	av;
	int	a,b,c,fp,fs;
	float	angle;

	InitEngine();

	if(part==1) USMP_SetOrder(0x8);

	View.Position.v1=0.0;
	View.Position.v2=0.0;
	View.Position.v3=100.0;
	View.Rotation.v1=0.0;
	View.Rotation.v2=0.0;
	View.Rotation.v3=0.0;
	View.FOV=512.0;
	View.Roll=0.0;

	av.v1=0.0;
	av.v2=0.0;
	av.v3=1250.0;
	CreateCubeTriFlat(av,250,18);	//20

	Rot[0].v1=0.0;
	Rot[0].v2=0.0;
	Rot[0].v3=0.0;

	fp=0;
	angle=0.0;
	timer_start=os_ticks_get();
	start_txt=os_ticks_get();
	while(1) {	//exitbuc==0){	//Order<0x09){
		Cls2(dst,(640*480)>>2,55);	//70);
/*
		Line2(dst,40,18,600,18,0);
		Line2(dst,40,440,600,440,0);
		Line2(dst,600,18,600,440,0);
		Line2(dst,40,18,40,440,0);
*/
		Line2(dst,0.0625*SCREEN_WIDTH,0.0375*SCREEN_HEIGHT,
			  0.9375*SCREEN_WIDTH,0.0375*SCREEN_HEIGHT,0);
		Line2(dst,0.0625*SCREEN_WIDTH,0.9166*SCREEN_HEIGHT,
			  0.9375*SCREEN_WIDTH,0.9166*SCREEN_HEIGHT,0);
		Line2(dst,0.9375*SCREEN_WIDTH,0.0375*SCREEN_HEIGHT,
			  0.9375*SCREEN_WIDTH,0.9166*SCREEN_HEIGHT,0);
		Line2(dst,0.0625*SCREEN_WIDTH,0.0375*SCREEN_HEIGHT,
			  0.0625*SCREEN_WIDTH,0.9166*SCREEN_HEIGHT,0);

		Rot[0].v1=angle*2;
		Rot[0].v2=angle;
		Rot[0].v3=angle;

		UpdateRot();
		if((Order==0x8 && (Row==0x0 || Row==0x3 || Row==0x20 || Row==0x23 || Row==0x3f)) ||
		   (Order==0x9 && (Row==0x0 || Row==0x2 || Row==0x20 || Row==0x23 || Row==0x26 ||
		                   Row==0x29 || Row==0x30 || Row==0x33 || Row==0x38 || Row==0x3B || Row==0x3E))){	
						    //3
			View.Position.v3=350.0;}
		else {View.Position.v3=100.0;}


		Render(&View,dst);

		angle=1.4*(os_ticks_get()-timer_start);
		//CubeScroller(fp,0,dst);
		fp=os_ticks_get();
		fs=((int)((os_ticks_get()-start_txt)/98))%53;
		DrawString(10,(0.027*SCREEN_HEIGHT),FontBuffer,dst,Messages[fs],1,1);
		if (os_fb_swap_buffers())
			dacube2_exit(0);
		if(part==1 && Order==0x09) break;
		if(part==2 && ((Order==0x09 && Row>62) || Order>=10)) break;
	}
}

void	Cubes8(unsigned char *dst)
{
	VECTOR	av;
	int	a,b,c,fp,fs,dir,oldf,nf;
	float	angle;

	USMP_SetOrder(0x09);
	InitEngine();

	View.Position.v1=0.0;
	View.Position.v2=-350.0;
	View.Position.v3=-400.0;	//-350
	View.Rotation.v1=350.0;
	View.Rotation.v2=0.0;
	View.Rotation.v3=0.0;
	View.FOV=100.0;
	View.Roll=0.0;

	av.v1=0.0;
	av.v2=-200.0;
	av.v3=0;
	CreateCubeTri(av,50,18);	//20

	av.v1=0.0;
	av.v2=-200.0;
	av.v3=200.0;
	CreateCubeTriFlat2(av,50,18);	//20

	av.v1=0.0;
	av.v2=-200.0;
	av.v3=200.0;
	CreateCubeTriFlat2(av,50,18);	//20

	av.v1=0.0;
	av.v2=-200.0;
	av.v3=200.0;
	CreateCubeTriFlat2(av,50,18);	//20

	Rot[0].v1=0.0;
	Rot[0].v2=0.0;
	Rot[0].v3=0.0;

	Rot[1].v1=0.0;
	Rot[1].v2=0.0;
	Rot[1].v3=0.0;

	Rot[2].v1=0.0;
	Rot[2].v2=0.0;
	Rot[2].v3=0.0;

	Rot[3].v1=0.0;
	Rot[3].v2=0.0;
	Rot[3].v3=0.0;

	for(a=0;a<6;a++) {
		angle=((float)(posy[a]))/480.0;
		posy[a]=angle*SCREEN_HEIGHT;
	}
	nf=0;
	angle=0.0;
	fp=0;
	timer_start=os_ticks_get();
	dir=1;
	oldf=-1;
	while(Order==0x09 && Row<0x39){
		Cls2(dst,(640*480)>>2,55);	//70);
/*
		Line2(dst,0,280,290,280,0);
		Line2(dst,360,280,640,280,0);
*/
	
		Line2(dst,0,0.583*SCREEN_HEIGHT,
			 0.453*SCREEN_WIDTH,0.583*SCREEN_HEIGHT,0);

		Line2(dst,0.5625*SCREEN_WIDTH,0.583*SCREEN_HEIGHT,
			 SCREEN_WIDTH-1,0.583*SCREEN_HEIGHT,0);

		//CubeScroller2(fp,200,0,dir,dst);

		Rot[0].v2=angle;
		Rot[1].v2=angle+120;
		Rot[2].v2=angle+240;
		Rot[3].v2=angle+360;

		UpdateRot();

		Render(&View,dst);

		if(Row==0x00 || Row==0x20 || Row==0x23 || Row==0x26 || 
		   Row==0x29 || Row==0x30 || Row==0x33 || Row==0x38 ||
		   Row==0x3B || Row==0x3E) {
			timer_start-=10;
			dir=(suiRand()%2);
			//y1=(suiRand()%400)+20;
			//y2=(suiRand()%400)+20;
		}

		angle=1.8*(os_ticks_get()-timer_start);	//1.4;

		fs=((int)((os_ticks_get()-start_txt)/98))%53;
		if(fs!=oldf){
			oldf=fs;
			nf++;
			if (nf > 5) {
				nf = 5;
			}
		}
//jarl
		for(a=0;a<nf;a++)
			DrawString(10,posy[a],FontBuffer,dst,Messages[fs-(nf-a)],2,1);

		fp=os_ticks_get();
		if (os_fb_swap_buffers())
			dacube2_exit(0);
	}
}

void	Greetings(unsigned char *dst)
{
	VECTOR	av;
	int	a,b,c,fp,fs,fs2,old,h,px,px2,sign;
	float	angle,timer_ts,fl1,fl2;
	
	USMP_SetOrder(0xD);

	InitEngine();
	
	View.Position.v1=-100.0;
	View.Position.v2=-100.0;
	View.Position.v3=100.0;
	View.Rotation.v1=0.0;
	View.Rotation.v2=0.0;
	View.Rotation.v3=0.0;
	View.FOV=256.0;
	View.Roll=0.0;
	c=0;
	for(a=0;a<6;a++){
		for(b=0;b<8;b++){
			av.v1=(b-4)*200;
			av.v2=(a-3)*200;
			av.v3=1000;	//1250.0;	//980
			CubOrg[c]=av;
			CreateCubeTri(av,80,18);	//20
			c++;
		}
	}

	for(a=0;a<6*8;a++){
		Rot[a].v1=a*12;	//0.0;
		Rot[a].v2=a*12;	//0.0;
		Rot[a].v3=a*12;	//8.0;
	}

	fp=0;
	fs2=0;
	old=0;
	h=(suiRand()%300)+75;
	px=(suiRand()%360)+100;
	px2=px;
	timer_start=((int) (os_ticks_get()+0.5));
	timer_ts=os_ticks_get();

	for(a=0;a<5;a++){
		os_fb_set_palette(245+a, fpal[a*3], fpal[a*3+1], fpal[a*3+2]);
	}
	sign=1;
	while(Order<0x10){
		Cls(dst,(640*480)>>2);		//70);
											  
		UpdateRot();
		Render(&View,dst);
		angle=(os_ticks_get()-timer_start);//*1.5;
		for(a=0;a<6*8;a++){
			Rot[a].v1=angle+a*8;//12;
			Rot[a].v2=angle+a*8;//12;
			Rot[a].v3=angle+a*8;//12;
		}

//		CubeScroller(fp,250,dst);
//		CubeScroller2(fp,420,250,1,0,dst);
		fp=os_ticks_get();

		fs2=((int)((os_ticks_get()-timer_start)/128))%18;
		if(fs2!=old){
			h=(suiRand()%300)+75;
			old=fs2;
			//px=(suiRand()%360)+80;
			px=310+(suiRand()%20);
			px2=px;
			sign=suiRand()%2;
			if(sign==0) sign=-1;
			timer_ts=os_ticks_get();
		}
		DrawString(px,h,FontBuffer,dst,Greets[fs2],2,0);	//jurl

		px=px2+((os_ticks_get()-timer_ts)/4)*sign;

		fl1=((os_ticks_get()-timer_ts)/75);
		if(fl1>1.0) fl1=1.0-(fl1-1.0);

		for(a=0;a<5;a++){
			os_fb_set_palette(251+a, ((int) (fpal[a*3  ]*fl1)), ((int) (fpal[a*3+1  ]*fl1)), ((int) (fpal[a*3+2  ]*fl1)));
		}

		fs=((int)((os_ticks_get()-start_txt)/98))%53;	//98
		DrawString2(10,(0.004*SCREEN_HEIGHT),FontBuffer,dst,Messages[fs],1,1);
				//2
		if (os_fb_swap_buffers())
			dacube2_exit(0);
	}
	for(a=0;a<5;a++){
		os_fb_set_palette(251+a, fpal[a*3  ], fpal[a*3+1  ], fpal[a*3 +2 ]);
	}
}


void	WaveCubes(unsigned char *dst)
{
	VECTOR	av;
	int	a,b,c,fp,fs;
	float	angle,factor;
	int	fs2;
	int	last;

	USMP_SetOrder(0x0B);

	InitEngine();
	
	View.Position.v1=-100.0;
	View.Position.v2=500.0;
	View.Position.v3=100.0;
	View.Rotation.v1=30.0;
	View.Rotation.v2=0.0;
	View.Rotation.v3=0.0;
	View.FOV=256.0;
	View.Roll=0.0;
	c=0;
	for(a=0;a<6;a++){
		for(b=0;b<6;b++){
			av.v1=(b-3)*200;
			av.v2=(a-3)*200;
			av.v3=1250.0;	//980
			CubOrg[c]=av;
			CreateCubeTri(av,80,18);	//20
			c++;
		}
	}

	for(a=0;a<6*6;a++){
		Rot[a].v1=0.0;
		Rot[a].v2=0.0;
		Rot[a].v3=0.0;
	}

	for(a=1;a<256;a++){
		auxpal[a*3  ]=os_fb_get_palette_red(a);
		auxpal[a*3+1]=os_fb_get_palette_green(a);
		auxpal[a*3+2]=os_fb_get_palette_blue(a);
	}

	angle=0;

	fp=0;
	timer_start=os_ticks_get();
	fs2=0;
	last=-1;
	while(Order<0x0D){
		Cls2(dst,(640*480)>>2,55);	//70);

		angle=(1.5*(os_ticks_get()-timer_start));
		fs2=((int)((os_ticks_get()-timer_start)/54))%25;
		
		c=0;
		for(a=0;a<6;a++){
			for(b=0;b<6;b++){				 
				Mov[c].v1=CubOrg[c].v1;
				Mov[c].v2=CubOrg[c].v2;
				Mov[c].v3=CubOrg[c].v3+340.0*fsin((angle+b*12+a*12)*PI/180.0);
				c++;
			}
		}
		UpdateRot();
		UpdateMov();

		Render(&View,dst);

		fs=((int)((os_ticks_get()-start_txt)/98))%53;	//98
		DrawString(10,(0.958*SCREEN_HEIGHT),FontBuffer,dst,Messages[fs],1,1);
			//460

		if((Order==0xB && (Row==0x1E || Row==0x20 || Row==0x22 || 
				  Row==0x30 || Row==0x3E))||
		   (Order==0xC && (Row==0x00 || Row==0x02 || Row==0x06 ||
				   Row==0x0A || Row==0x0E || Row==0x10 ||
				   Row==0x12 || Row==0x16 || Row==0x1A ||
				   Row==0x22 || Row==0x26 || Row==0x2A ||
				   Row==0x2E || Row==0x30 || Row==0x32 ||
				   Row==0x36 || Row==0x3A || Row==0x3E)))  {
					timer_start-=8;
				}
		if(Order==0xC && (Row==0x04 || Row==0x0C || Row==0x14 ||
				  Row==0x1C || Row==0x1F || Row==0x24 ||
				  Row==0x2C || Row==0x34)) {
			if(last!=Row){
				last=Row;
				for(a=1;a<256;a++){
					os_fb_set_palette(a, 63-auxpal[a*3  ], 
					63-auxpal[a*3 +1 ], 63-auxpal[a*3 +2  ]);
				}
			}
		} else {
			for(a=1;a<256;a++){
				os_fb_set_palette(a, auxpal[a*3  ], 
				auxpal[a*3 +1 ],auxpal[a*3 +2  ]);
			}
		}
		angle=0.4*(os_ticks_get()-timer_start);
		fp=os_ticks_get();
		if (os_fb_swap_buffers())
			dacube2_exit(0);
	}
	#warning leave palette in a sane state
	for(a=1;a<256;a++){
		os_fb_set_palette(a, auxpal[a*3  ], 
			auxpal[a*3 +1 ],auxpal[a*3 +2  ]);
	}
}

void Last(unsigned char *dst)
{
	VECTOR	av;
	int	a,b,c,fp,fs,rnz;
	unsigned int c1,c2;
	int	last;
	float	angle,factor;
	int	fs2,pos;
	int	nbum;
	int	timer_16;
//	char	*sph;
//	char	*k="kill the";
//	char	*s="spheres";

	USMP_SetOrder(0x12);

	for(a=0;a<256;a++){
		paleta[a*3  ]=os_fb_get_palette_red(a);
		paleta[a*3+1]=os_fb_get_palette_green(a);
		paleta[a*3+2]=os_fb_get_palette_blue(a);
	}

	InitEngine();

	//sph=(char *) &sphere;
	
	View.Position.v1=-100.0;
	View.Position.v2=-800.0;
	View.Position.v3=100.0;
	View.Rotation.v1=330.0;
	View.Rotation.v2=0.0;
	View.Rotation.v3=0.0;
	View.FOV=256.0;
	View.Roll=0.0;
	c=0;
	last=-1;
	oldseed=0x1234;

	for(a=0;a<6;a++){
		for(b=0;b<4;b++){
			av.v1=(b-2)*350;
			av.v2=(a-3)*200;
			av.v3=1250.0;	//980
			CubOrg[c]=av;
			CreateCubeTri(av,80,18);	//20
			c++;
		}
	}

	angle=0;

	fp=0;
	timer_start=os_ticks_get();
	fs2=0;
	nbum=0;
	while(Order<0x16){ 	//!(Order==0x19 && Row>0x30)){
		Cls2(dst,(640*480)>>2,55);	//70);

		angle=2*(os_ticks_get()-timer_start);

		fs2=((int)((os_ticks_get()-timer_start)/74))%25;

		c=0;
		for(a=0;a<6;a++){
			for(b=0;b<4;b++){
				if(vpos[c]){
					Mov[c].v1=CubOrg[c].v1+300*fcos((angle+b*12+a*12)*PI/180.0);
					Mov[c].v2=CubOrg[c].v2+100*fsin((angle+b*12+a*12)*PI/180.0);
					Mov[c].v3=CubOrg[c].v3;
					Rot[c].v3=angle+b;
					Rot[c].v2=angle+a;
				} else {
					Mov[c].v1=CubOrg[c].v1;
					Mov[c].v2=CubOrg[c].v2;
					Mov[c].v3=CubOrg[c].v3+500.0;
					Rot[c].v3=angle;
				}
				c++;
			}
		}

		UpdateRot();
		UpdateMov();

		Render(&View,dst);

		DrawString(10,(0.468*SCREEN_HEIGHT),FontBuffer,dst,messages2[fs2],2,1);
			//225

		if(((Order==0x12 || Order==0x13 || Order==0x16 || Order==0x17) &&
		       (Row==0x08 || Row==0x12 || Row==0x18 || Row==0x28 || Row==0x32 || 
		 	Row==0x38 || Row==0x3C)) || 
		   ((Order==0x14 || Order==0x18) && (Row==0x08 || Row==0x18 || 
			Row==0x28 || Row==0x32 || Row==0x38)) ||
		   ((Order==0x15 || Order==0x19) && (Row==0x08 || Row==0x12 || 
			Row==0x18 || Row==0x28 || Row==0x3F))){

			if(last!=Row){
				for(a=0;a<6*4;a++){
					rnz=suiRand()%10;
					if(rnz<6) vpos[a]=1;
					else vpos[a]=0;
				}
				last=Row;
			}
		}

		if(Order==0x15 && (Row==0x30 || Row==0x36 || Row==0x3C))
			Cls(dst,(640*480)>>2);

		angle=2*(os_ticks_get()-timer_start);
		fp=os_ticks_get();
		if (os_fb_swap_buffers())
			dacube2_exit(0);
	}
}

void	CubesInside(unsigned char *dst)
{
	VECTOR	av;
	int	a,b,c,e,fp,fs,fs2,old,h,px,px2;
	float	angle,timer_ts,fl1,fl2;
	int	OldRow;
	
	USMP_SetOrder(0x10);	//?

	OldRow=0x10;

	InitEngine();

	View.Position.v1=0.0;
	View.Position.v2=0.0;
	View.Position.v3=-500.0;
	View.Rotation.v1=0.0;
	View.Rotation.v2=0.0;
	View.Rotation.v3=0.0;
	View.FOV=512.0;
	View.Roll=0.0;
	for(a=0;a<5;a++){
		av.v1=0.0;
		av.v2=0.0;
		av.v3=900.0;
		//CreateCubeTri(av,radius[a],18);	//(128-color[a])/2);
		CreateCubeW(av,radius[a],color[a]);
	}

	for(a=0;a<5;a++){
		Rot[a].v1=a*12;	//0.0;
		Rot[a].v2=a*12;	//0.0;
		Rot[a].v3=a*12;	//8.0;
	}

	fp=0;
	fs2=0;
	old=0;
	h=(suiRand()%300)+75;
	px=(suiRand()%360)+100;
	px2=px;
	timer_start=((int) (os_ticks_get()+0.5));
	timer_ts=os_ticks_get();
	while(Order<0x12){
		Cls2(dst,(640*480)>>2,70);

		UpdateRot();
		Render(&View,dst);
		angle=(os_ticks_get()-timer_start)*1.5;
		for(a=0;a<5;a++){
			Rot[a].v1=angle+a*12;
			Rot[a].v2=angle+a*12;
			Rot[a].v3=angle+a*12;
		}
		fs=((int)((os_ticks_get()-start_txt)/98))%53;	//98
		DrawString(10,(0.958*SCREEN_HEIGHT),FontBuffer,dst,Messages[fs],1,1);
				//460
		fp=os_ticks_get();

		if(Order==0x11 && OldRow==0x10){
			InitEngine();
			OldRow=0x11;
			for(a=0;a<5;a++){
				av.v1=0.0;
				av.v2=0.0;
				av.v3=900.0;
				CreateCubeTri(av,radius[a],18);	//(128-color[a])/2);
			}
		}
		if (os_fb_swap_buffers())
			dacube2_exit(0);
	}
}

void	Last2(unsigned char *dst)
{
	VECTOR	av;
	int	a,b,c,fp,fs,pos;
	float	angle,fl1;

	USMP_SetOrder(0x1A);

	for(a=0;a<256;a++){
		os_fb_set_palette(a, 0, 0, 0);
	}

	for(a=245;a<251;a++){
		os_fb_set_palette(a, fpal[(a-245)*3  ], fpal[(a-245)*3+1  ], fpal[(a-245)*3+2  ]);
	}

	fp=0;
	timer_start=os_ticks_get();
	while(Order>0x19){
		Cls(dst,(640*480)>>2);		//70);

		fl1=(((float)(os_ticks_get()-timer_start))/128.0)+0.5;
		if(fl1>1.0) fl1=1.0-(fl1-1.0);
		if(fl1<0.5) {fl1=0.5;timer_start=os_ticks_get();}

		for(a=0;a<5;a++){
			os_fb_set_palette(251+a, ((int) (fpal[a*3  ]*fl1)), 
			((int) (fpal[a*3+1  ]*fl1)), ((int) (fpal[a*3+2  ]*fl1)));
		}
		pos=((int)(os_ticks_get())) & 0x40;
		DrawString(10,(0.468*SCREEN_HEIGHT),FontBuffer,dst,LastM,2,1);
			//225
		fp=os_ticks_get();
		DrawString2(10,(0.958*SCREEN_HEIGHT),FontBuffer,dst,frase,1,2);
			//460
		if (os_fb_swap_buffers())
			dacube2_exit(0);
	}
} 

void ReLast(unsigned char *dst)
{
	VECTOR	av;
	int	a,b,c,fp,fs,rnz;
	unsigned int c1,c2;
	int	last;
	float	angle,factor;
	int	fs2,pos;
	int	nbum;
	int	timer_16;
	int	factor1,factor2,factor3;
//	char	*sph;
//	char	*k="kill the";
//	char	*s="spheres";

	USMP_SetOrder(0x16);

	for(a=0;a<256;a++){
                paleta[a*3  ]=os_fb_get_palette_red(a);
		paleta[a*3+1]=os_fb_get_palette_green(a);
		paleta[a*3+2]=os_fb_get_palette_blue(a);
	}

	InitEngine();

	factor1=factor2=factor3=0;

	//sph=(char *) &sphere;
/*	
	View.Position.v1=0.0;
	View.Position.v2=0.0;
	View.Position.v3=-2000.0;
	View.Rotation.v1=0.0;
	View.Rotation.v2=0.0;
	View.Rotation.v3=0.0;
	View.FOV=512.0;
	View.Roll=0.0;
*/
	View.Position.v1=0.0;
	View.Position.v2=100.0;
	View.Position.v3=-1000.0;	//-350
	View.Rotation.v1=30.0;
	View.Rotation.v2=0.0;
	View.Rotation.v3=0.0;
	View.FOV=512.0;
	View.Roll=0.0;

	c=0;
	last=-1;

	av.v1=0.0;
	av.v2=-100.0;
	av.v3=0;
	CreateCubeTri(av,50,18);	//20

	av.v1=0.0;
	av.v2=-100.0;
	av.v3=200.0;
	CreateCubeTriFlat2(av,50,18);	//20

	av.v1=0.0;
	av.v2=-100.0;
	av.v3=200.0;
	CreateCubeTriFlat2(av,50,18);	//20

	av.v1=0.0;
	av.v2=-100.0;
	av.v3=200.0;
	CreateCubeTriFlat2(av,50,18);	//20

	av.v1=0.0;
	av.v2=100.0;
	av.v3=0;
	CreateCubeTri(av,50,18);	//20

	av.v1=0.0;
	av.v2=100.0;
	av.v3=200.0;
	CreateCubeTriFlat2(av,50,18);	//20

	av.v1=0.0;
	av.v2=100.0;
	av.v3=200.0;
	CreateCubeTriFlat2(av,50,18);	//20

	av.v1=0.0;
	av.v2=100.0;
	av.v3=200.0;
	CreateCubeTriFlat2(av,50,18);	//20

	av.v1=0.0;
	av.v2=300.0;
	av.v3=0.0;
	CreateCubeTri(av,50,18);	//20

	av.v1=0.0;
	av.v2=300.0;
	av.v3=200.0;
	CreateCubeTriFlat2(av,50,18);	//20

	av.v1=0.0;
	av.v2=300.0;
	av.v3=200.0;
	CreateCubeTriFlat2(av,50,18);	//20

	av.v1=0.0;
	av.v2=300.0;
	av.v3=200.0;
	CreateCubeTriFlat2(av,50,18);	//20

	for(a=0;a<8;a++){
		Rot[a].v1=0.0;
		Rot[a].v2=0.0;
		Rot[a].v3=0.0;
	}

	angle=0;

	fp=0;
	timer_start=os_ticks_get();
	fs2=0;
	nbum=0;
	while(!(Order==0x19 && Row>0x30)){
		Cls2(dst,(640*480)>>2,55);	//70);

		angle=2*(os_ticks_get()-timer_start);

		fs2=((int)((os_ticks_get()-timer_start)/74))%25;

		Rot[0].v2=angle;
		Rot[1].v2=angle+120*factor1;
		Rot[2].v2=angle+240*factor1;
		Rot[3].v2=angle+360*factor1;

		Rot[4].v2=-(angle+45);
		Rot[5].v2=-(angle+45+120*factor2);
		Rot[6].v2=-(angle+45+240*factor2);
		Rot[7].v2=-(angle+45+360*factor2);

		Rot[8].v2=angle+45*2;
		Rot[9].v2=angle+45*2+120*factor1;
		Rot[10].v2=angle+45*2+240*factor1;
		Rot[11].v2=angle+45*2+360*factor1;

		UpdateRot();
		Render(&View,dst);

		DrawString(10,(0.875*SCREEN_HEIGHT),FontBuffer,dst,messages2[fs2],2,1);	
			//420

		if( ((Order==0x16 || Order==0x17) &&
		       (Row==0x08 || Row==0x12 || Row==0x18 || Row==0x28 || Row==0x32 || 
		 	Row==0x38 || Row==0x3C)) || 
		   (Order==0x18 && (Row==0x08 || Row==0x18 || 
			Row==0x28 || Row==0x32 || Row==0x38)) ||
		   (Order==0x19 && (Row==0x08 || Row==0x12 || 
			Row==0x18 || Row==0x28 || Row==0x3F))){

			if(last!=Row){
				factor1++;
				factor2+=2;
				last=Row;
				for(a=1;a<256;a++){
                                        auxpal[a*3  ]=os_fb_get_palette_red(a);
					auxpal[a*3+1]=os_fb_get_palette_green(a);
					auxpal[a*3+2]=os_fb_get_palette_blue(a);
				}

				for(a=1;a<256;a++){
					os_fb_set_palette(a, 63-auxpal[a*3  ], 63-auxpal[a*3+1  ],
						63-auxpal[a*3 +2 ]);
				}

			}
		}

		if(Order==0x19)
		if(Row<0x30) {
			if(Row%3==0){
				for(a=0;a<256;a++){
					os_fb_set_palette(a, 63, 63, 63);
				}
			} else {
				for(a=0;a<256;a++){
					os_fb_set_palette(a, paleta[a*3  ], paleta[a*3 +1 ],
						paleta[a*3 +2]);

				}
			}
		} else {
			if(Row>0x2F && Row<0x3B && Row!=0x33 && Row!=0x35 && Row!=0x39){
				for(a=0;a<256;a++){
					os_fb_set_palette(a, 63, 63, 63);					
				}
			} else {
				for(a=0;a<256;a++){
					os_fb_set_palette(a, paleta[a*3  ], paleta[a*3 +1 ],
						paleta[a*3 +2]);
				}
			}
		}
		angle=2*(os_ticks_get()-timer_start);
		fp=os_ticks_get();
		if (os_fb_swap_buffers())
			dacube2_exit(0);
	}
}

