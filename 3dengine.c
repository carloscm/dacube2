#include <stdio.h>
#include <stdlib.h>
#include <math.h>

#include "oldskool.h"

#include "struct.h"

#warning put multires support here
#define SCREEN_WIDTH 640
#define SCREEN_HEIGHT 480

#define	DTR	0.01745329252

VECTOR	Rot[80];
VECTOR	Mov[80];

int	table[512];

int	o_idx;
int	nf;
int	sizes[4]={5,8,10,12};
int	sizes2[4]={8,11,14,16};
float	fx,fy;
int	maxx,maxy;
float	centerx,centery;

float	Cnt65536=65536.0;

#define fistshl(__a,__b) (*__a)=((__b)*65536.f)
#define fist(__a,__b) (*__a)=(__b)

#ifndef WIN32
#define fsin sinf
#define fcos cosf
#else
#define fsin sin
#define fcos cos
#endif
#define ffabs fabs
#define fsqrt sqrt

OBJECT	Objects[80];

void	inittable()
{
	int	a;

	for(a=0;a<250;a++){
		table[a]=a;
	}

	for(a=250;a<512;a++){
		table[a]=250;
	}
}

void	VxM (VECTOR *b,MATRIX *a,VECTOR *r)
{
	VECTOR	r2;

	r2.v1=a->m11*b->v1+a->m21*b->v2+a->m31*b->v3;
	r2.v2=a->m12*b->v1+a->m22*b->v2+a->m32*b->v3;
	r2.v3=a->m13*b->v1+a->m23*b->v2+a->m33*b->v3;
	r->v1=r2.v1;
	r->v2=r2.v2;
	r->v3=r2.v3;
}

void	CalcTransformM(VECTOR *Rot,MATRIX *Trans)
{
	float	sx,sy,sz,cx,cy,cz;
	float	r1,r2,r3;

	r1=Rot->v1*DTR;
	r2=Rot->v2*DTR;
	r3=Rot->v3*DTR;

	sx=fsin(r1);
	sy=fsin(r2);
	sz=fsin(r3);
	cx=fcos(r1);
	cy=fcos(r2);
	cz=fcos(r3);

	Trans->m11=cy*cz;
	Trans->m12=sx*sy*cz-cx*sz;
	Trans->m13=cx*sy*cz+sx*sz;
	Trans->m21=cy*sz;
	Trans->m22=sx*sy*sz+cx*cz;
	Trans->m23=cx*sy*sz-sx*cz;
	Trans->m31=-sy;
	Trans->m32=sx*cy;
	Trans->m33=cx*cy;
}


void Line2Primperan(unsigned char *dst,float x1f,float y1f,float x2f,float y2f,char color){
	int 	pos,inc,dex,dey,xp,yp,offs;
	int 	alias,antialias;
	int 	x1,x2,y1,y2;
	int 	minx,miny,maxx,maxy;
	unsigned int	aliase;
	unsigned int	aaliase;
	unsigned int	f00,f01,f10,f11;
	int	count,c1,c2;
        float   dx,dy;
	float	m1,m2;
	float ymax = 479.f, ymin = 0.f;
	float xmax = 639.f, xmin = 0.f;
	
	c1=((x1f<xmin)<<3) | ((x1f>xmax)<<2) | ((y1f<ymin)<<1) | ((y1f>ymax)<<0);
	c2=((x2f<xmin)<<3) | ((x2f>xmax)<<2) | ((y2f<ymin)<<1) | ((y2f>ymax)<<0);
	while (c1 | c2){
		if(c1 & c2) return;
		dx=x2f-x1f;
		dy=y2f-y1f;
		m1=dy/dx;
		m2=dx/dy;

		if(c1){
			if(c1&8) {y1f+=(xmin-x1f)*m1;x1f=xmin;} else 
			if(c1&4) {y1f+=(xmax-x1f)*m1;x1f=xmax;} else 
			if(c1&2) {x1f+=(ymin-y1f)*m2;y1f=ymin;} else 
			if(c1&1) {x1f+=(ymax-y1f)*m2;y1f=ymax;}
			c1=((x1f<xmin)<<3)|((x1f>xmax)<<2)|((y1f<ymin)<<1)|((y1f>ymax));
		} else {
			if(c2&8) {y2f+=(xmin-x2f)*m1;x2f=xmin;} else 
			if(c2&4) {y2f+=(xmax-x2f)*m1;x2f=xmax;} else 
			if(c2&2) {x2f+=(ymin-y2f)*m2;y2f=ymin;} else 
			if(c2&1) {x2f+=(ymax-y2f)*m2;y2f=ymax;}
			c2=((x2f<xmin)<<3)|((x2f>xmax)<<2)|((y2f<ymin)<<1)|((y2f>ymax));
		}
	}

	if(x2f-x1f==0.0 && y2f-x1f==0.0) return;
	if(ffabs(x2f-x1f)>ffabs(y2f-y1f)){
		x1=((int) (65536.0*x1f));
		x2=((int) (65536.0*x2f));
		y1=((int) (65536.0*y1f));
		y2=((int) (65536.0*y2f));
	
		minx=x1;maxx=x2;miny=y1;maxy=y2;
		if(maxx<minx){
			minx=x2;maxx=x1;miny=y2;maxy=y1;
		}

		
		pos=miny;

		dex=(maxx-minx);
		dey=(maxy-miny);
		if((dex>>16)==0) return;

		inc=dey/(dex>>16);
		aaliase=(minx>>8)&0xff;
		aliase=0x100-aaliase;

		xp=(minx)>>16;

		count=(maxx-minx+0x7fff)>>16;
		while(count) {
			yp=pos>>16;
//			if (xp < 0 || xp >= 640 || yp < 0 || yp >= 480)
//				break;

			antialias=(pos>>8)&0xff;
			alias=0x100-antialias;
		
			f00=(alias*aliase);
			f01=(aaliase*alias);
			f10=(aliase*antialias);
			f11=(aaliase*antialias);

			offs=(yp<<9)+(yp<<7)+xp;
			dst[offs]=((f00*(color-dst[offs]))>>16)+dst[offs];
			offs++;
			dst[offs]=((f01*(color-dst[offs]))>>16)+dst[offs];
			offs+=639;
//			if (yp == 479)
//				break;
			dst[offs]=((f10*(color-dst[offs]))>>16)+dst[offs];
			offs++;
			dst[offs]=((f11*(color-dst[offs]))>>16)+dst[offs];
			pos+=inc;
			xp++;
			count--;
		}
	} else {
		x1=((int) (65536.0*x1f));
		x2=((int) (65536.0*x2f));
		y1=((int) (65536.0*y1f));
		y2=((int) (65536.0*y2f));

		minx=x1;maxx=x2;miny=y1;maxy=y2;
		if(maxy<miny){
			minx=x2;maxx=x1;miny=y2;maxy=y1;
		}

	
		pos=minx;

		dex=(maxx-minx);
		dey=(maxy-miny);
		if((dey>>16)==0) return;

		inc=dex/(dey>>16);
		aaliase=(miny>>8)&0xff;
		aliase=0x100-aaliase;

		count=(maxy-miny+0x7fff)>>16;

		yp=(miny)>>16;
		while(count){
			xp=pos>>16;
//			if (xp < 0 || xp >= 640 || yp < 0 || yp >= 480)
//				break;

			antialias=(pos>>8)&0xff;
			alias=0x100-antialias;

			f00=(alias*aliase);
			f01=(antialias*aliase);
			f10=(alias*aaliase);
			f11=(antialias*aaliase);

			offs=(yp<<9)+(yp<<7)+xp;

			dst[offs]=((f00*(color-dst[offs]))>>16)+dst[offs];
			offs++;
			dst[offs]=((f01*(color-dst[offs]))>>16)+dst[offs];
			offs+=639;
//			if (yp == 479)
//				break;
			dst[offs]=((f10*(color-dst[offs]))>>16)+dst[offs];
			offs++;
			dst[offs]=((f11*(color-dst[offs]))>>16)+dst[offs];
			pos+=inc;
			yp++;
			count--;
		}
	}
}

void Line2(unsigned char *dst,float x1f,float y1f,float x2f,float y2f,int color)
{
	int pos,inc,inc2,dex,dey,xp,yp,offs;
	int alias,antialias;	//,xi,xf,yi,yf;
	int x1,x2,y1,y2;
/*
	fist(&x1,x1f);
	fist(&x2,x2f);
	fist(&y1,y1f);
	fist(&y2,y2f);
*/
	x1=x1f;
	x2=x2f;
	y1=y1f;
	y2=y2f;

    	dex=x2-x1;
    	dey=y2-y1;

	if(dex==0 && dey==0) return;
	if (ffabs(dex)>ffabs(dey)){
        	pos=y1<<16;
        	inc=(dey<<16)/dex;
        	if (dex>0) inc2=1;
        	else {inc2=-1; inc=-inc;}
        	xp=x1;
        	do  {
            		yp=pos>>16;
			if(yp>1 && xp>1 && xp<maxx && yp<maxy){
				antialias=pos&0xffff;
				alias=0x10000-antialias;
				offs=yp*SCREEN_WIDTH+xp;

            			dst[offs]=(color*alias+dst[offs]*antialias)>>16;
            			dst[offs+SCREEN_WIDTH]=(color*antialias+dst[offs+SCREEN_WIDTH]*alias)>>16;

			}
	            	pos+=inc;
        	    	xp+=inc2;
        	}while (xp!=x2);
    	} else {
        	pos=x1<<16;
        	inc=(dex<<16)/dey;
	        if (dey>0) inc2=1;
	        else {inc2=-1;inc=-inc;}
        	yp=y1;
	        do {
			xp=pos>>16;
			if(yp>1 && xp>1 && xp<maxx && yp<maxy){
				antialias=pos&0xffff;
				alias=0x10000-antialias;
				offs=yp*SCREEN_WIDTH+xp;	

            			dst[offs]=(color*alias+dst[offs]*antialias)>>16;
            			dst[offs+1]=(color*antialias+dst[offs+1]*alias)>>16;

			}
			pos+=inc;
			yp+=inc2;
		} while (yp!=y2);
	}
}

void DrawWireQuad(FACE3D *f,unsigned char *dst,int Color)
{
	Line2(dst,f->Vp[0].v1,f->Vp[0].v2,f->Vp[1].v1,f->Vp[1].v2,Color);
	Line2(dst,f->Vp[1].v1,f->Vp[1].v2,f->Vp[2].v1,f->Vp[2].v2,Color);
	Line2(dst,f->Vp[2].v1,f->Vp[2].v2,f->Vp[3].v1,f->Vp[3].v2,Color);
}

void DrawWireQuadPrimperan(FACE3D *f,unsigned char *dst,int Color)
{
	Line2Primperan(dst,f->Vp[0].v1,f->Vp[0].v2,f->Vp[1].v1,f->Vp[1].v2,Color);
	Line2Primperan(dst,f->Vp[1].v1,f->Vp[1].v2,f->Vp[2].v1,f->Vp[2].v2,Color);
	Line2Primperan(dst,f->Vp[2].v1,f->Vp[2].v2,f->Vp[3].v1,f->Vp[3].v2,Color);
}

void DrawWireQuad_(FACE3D *f,unsigned char *dst,int Color)
{
	Line2(dst,f->Vp[0].v1,f->Vp[0].v2,f->Vp[1].v1,f->Vp[1].v2,Color);
	Line2(dst,f->Vp[1].v1,f->Vp[1].v2,f->Vp[2].v1,f->Vp[2].v2,Color);
	Line2(dst,f->Vp[2].v1,f->Vp[2].v2,f->Vp[3].v1,f->Vp[3].v2,Color);
	Line2(dst,f->Vp[3].v1,f->Vp[3].v2,f->Vp[0].v1,f->Vp[0].v2,Color);
}

#if 1
void FlatAddInner(int Color,int Width,unsigned char *p) {
	while (Width > 0) {
		*p += Color;
		--Width;
		++p;
	}
}
#else
void FlatAddInner(int Color,int Width,unsigned char *);
#pragma	aux FlatAddInner=\
"cmp	ecx,0"\
"jle	@NoBucle"\
"mov	ebx,edi"\
"and	ebx,3"\
"jz	@Aligned"\
"add	ebx,-4"\
"neg	ebx"\
"@AlignB:"\
"add	[edi],al"\
"inc	edi"\
"dec	ecx"\
"jz	@NoByte"\
"dec	ebx"\
"jnz	@AlignB"\
"@Aligned:"\
"mov	ebx,ecx"\
"shr	ecx,2"\
"jz	@SomeBytes"\
"mov	ah,al"\
"push	ax"\
"shl	eax,16"\
"pop	ax"\
"@Hltr4:"\
"add	[edi],eax"\
"add	edi,4"\
"dec	ecx"\
"jnz	@Hltr4"\
"@SomeBytes:"\
"and	ebx,3"\
"jz	@NoByte"\
"@EndBytes:"\
"add	[edi],al"\
"inc	edi"\
"dec	ebx"\
"jnz	@EndBytes"\
"@NoByte:"\
"@NoBucle:"\
modify	[eax ecx edi ebx]\
parm	[eax] [ecx] [edi]
#endif

void DrawFlatAdd(FACE3D *f,unsigned char *dst,int Color)
{
        int     t,dx12,dx23,dx13,dxl,dxr,xpl,xpr,xa,xb,dxl2,dxr2,xp;
        int     y2y1,y3y2,y3y1;
	int	ys;

	int	tmp1;
	unsigned char	*tmp2;
        int	pos;
        int	pos2;
        int     x1,y1,x2,y2,x3,y3;

	x1=((int) (65536.0*f->Vp[0].v1));
	x2=((int) (65536.0*f->Vp[1].v1));
	x3=((int) (65536.0*f->Vp[2].v1));
	y1=((int) (f->Vp[0].v2));
	y2=((int) (f->Vp[1].v2));
	y3=((int) (f->Vp[2].v2));
/*
	fistshl(&x1,f->Vp[0].v1);
	fistshl(&x2,f->Vp[1].v1);
	fistshl(&x3,f->Vp[2].v1);
	fist(&y1,f->Vp[0].v2);
	fist(&y2,f->Vp[1].v2);
	fist(&y3,f->Vp[2].v2);
*/
        if (y1>y2)      {t=y1; y1=y2; y2=t; t=x1; x1=x2; x2=t;}
        if (y1>y3)      {t=y1; y1=y3; y3=t; t=x1; x1=x3; x3=t;}
        if (y2>y3)      {t=y2; y2=y3; y3=t; t=x2; x2=x3; x3=t;}

	if(y1>maxy) return;
	if(y3<1) return;

        dx12=x2-x1;     dx23=x3-x2;     dx13=x3-x1;

        y2y1=y2-y1;
        y3y1=y3-y1;
        y3y2=y3-y2;

        if (y2y1)      {dx12/=y2y1;}
        if (y3y2)      {dx23/=y3y2;}
        if (y3y1)      {dx13/=y3y1;}

        if (dx12>dx13) {dxl=dx13;    dxr=dx12;    dxl2=dx13;   dxr2=dx23;}
        else 	       {dxl=dx12;    dxr=dx13;    dxl2=dx23;   dxr2=dx13;}

        pos=y1*SCREEN_WIDTH;
        xpr=xpl=x1;

	if(y1<1){
		xpr+=dxr*(-y1);
		xpl+=dxl*(-y1);
		pos=SCREEN_WIDTH;
		y1=1;
	}

	if(y2>=1){
		if(y2<maxy+1) ys=y2;
		else ys=maxy+1;

	        for (;y1<y2;y1++)
        	{
	                xb=(xpr>>16);
			if(xb>1) {
	                	xa=(xpl>>16);
				if(xa<maxx) {
					if(xb>maxx) xb=maxx;
					if(xa<1) xa=1;
			                pos2=pos+xa;
					tmp1=xb-xa;
					tmp2=(unsigned char *) (dst+pos2);
					if ((pos2+tmp1) > 640*480)
						tmp1 = 640*480 - pos2;
					FlatAddInner(Color,tmp1,tmp2);
					//for(xp=xa;xp<xb;xp++) {dst[pos2]+=Color;pos2++;}
				}
			}

	                xpl+=dxl;
        	        xpr+=dxr;
	                pos+=SCREEN_WIDTH;
        	}
	        if(dx12>dx13){
        	        xpr=x2;
	        } else {
        	        xpl=x2;
	        }
	} else {
	        if(dx12>dx13){
        	        xpr=x2;
	        } else {
        	        xpl=x2;
	        }
		xpr+=dxr2*(-y2);
		xpl+=dxl2*(-y2);
		pos=SCREEN_WIDTH;
		y1=1;		
	}

	if(y3<maxy+1) ys=y3;
	else ys=maxy+1;

        for (;y1<ys;y1++)
        {
                xb=(xpr>>16);
		if(xb>1){
	                xa=(xpl>>16);
			if(xa<maxx) {
				if(xb>maxx) xb=maxx;
				if(xa<1) xa=1;
	                	pos2=pos+xa;
				tmp1=xb-xa;
				tmp2=(unsigned char *) (dst+pos2);
				if ((pos2+tmp1) > 640*480)
					tmp1 = 640*480 - pos2;
				FlatAddInner(Color,tmp1,tmp2);
				//for(xp=xa;xp<xb;xp++) {dst[pos2]+=Color;pos2++;}

			}
		}
                xpl+=dxl2;
                xpr+=dxr2;
                pos+=SCREEN_WIDTH;
        }
	if(nf%2==0) DrawWireQuad_(f,dst,35);	//jurl
}

void DrawFlat(FACE3D *f,unsigned char *dst,int Color)
{
        int     t,dx12,dx23,dx13,dxl,dxr,xpl,xpr,xa,xb,dxl2,dxr2,xp;
        int     y2y1,y3y2,y3y1;
	int	ys;

	int	tmp1;
	unsigned char	*tmp2;
        int	pos;
        int	pos2;
        int     x1,y1,x2,y2,x3,y3;
	int	col;

	x1=((int) (65536.0*f->Vp[0].v1));
	x2=((int) (65536.0*f->Vp[1].v1));
	x3=((int) (65536.0*f->Vp[2].v1));
	y1=((int) (f->Vp[0].v2));
	y2=((int) (f->Vp[1].v2));
	y3=((int) (f->Vp[2].v2));
	col=f->Color;

        if (y1>y2)      {t=y1; y1=y2; y2=t; t=x1; x1=x2; x2=t;}
        if (y1>y3)      {t=y1; y1=y3; y3=t; t=x1; x1=x3; x3=t;}
        if (y2>y3)      {t=y2; y2=y3; y3=t; t=x2; x2=x3; x3=t;}

	if(y1>maxy) return;
	if(y3<1) return;

        dx12=x2-x1;     dx23=x3-x2;     dx13=x3-x1;

        y2y1=y2-y1;
        y3y1=y3-y1;
        y3y2=y3-y2;

        if (y2y1)      {dx12/=y2y1;}
        if (y3y2)      {dx23/=y3y2;}
        if (y3y1)      {dx13/=y3y1;}

        if (dx12>dx13) {dxl=dx13;    dxr=dx12;    dxl2=dx13;   dxr2=dx23;}
        else 	       {dxl=dx12;    dxr=dx13;    dxl2=dx23;   dxr2=dx13;}

        pos=y1*SCREEN_WIDTH;
        xpr=xpl=x1;

	if(y1<1){
		xpr+=dxr*(-y1);
		xpl+=dxl*(-y1);
		pos=SCREEN_WIDTH;
		y1=1;
	}

	if(y2>=1){
		if(y2<maxy+1) ys=y2;
		else ys=maxy+1;

	        for (;y1<ys;y1++)
        	{
	                xb=(xpr>>16);
			if(xb>1) {
	                	xa=(xpl>>16);
				if(xa<maxx) {
					if(xb>maxx) xb=maxx;
					if(xa<1) xa=1;
			                pos2=pos+xa;
					tmp1=xb-xa;
					tmp2=(unsigned char *) (dst+pos2);
					if ((pos2+tmp1) > 640*480)
						tmp1 = 640*480 - pos2;
					FlatAddInner(col,tmp1,tmp2);	//c
/*
					for(xp=xa;xp<xb;xp++) {
						dst[pos2]=table[dst[pos2]+col];
						pos2++;
					}
*/
				}
			}

	                xpl+=dxl;
        	        xpr+=dxr;
	                pos+=SCREEN_WIDTH;
        	}
	        if(dx12>dx13){
        	        xpr=x2;
	        } else {
        	        xpl=x2;
	        }
	} else {
	        if(dx12>dx13){
        	        xpr=x2;
	        } else {
        	        xpl=x2;
	        }
		xpr+=dxr2*(-y2);
		xpl+=dxl2*(-y2);
		pos=SCREEN_WIDTH;
		y1=1;		
	}

	if(y3<maxy+1) ys=y3;
	else ys=maxy+1;

        for (;y1<ys;y1++)
        {
                xb=(xpr>>16);
		if(xb>1){
	                xa=(xpl>>16);
			if(xa<maxx) {
				if(xb>maxx) xb=maxx;
				if(xa<1) xa=1;
	                	pos2=pos+xa;
				tmp1=xb-xa;
				tmp2=(unsigned char *) (dst+pos2);
				if ((pos2+tmp1) > 640*480)
					tmp1 = 640*480 - pos2;
				FlatAddInner(col,tmp1,tmp2);	//c
/*
				for(xp=xa;xp<xb;xp++) {
					dst[pos2]=table[dst[pos2]+col];
					pos2++;
				}	
*/
			}
		}
                xpl+=dxl2;
                xpr+=dxr2;
                pos+=SCREEN_WIDTH;
        }
}

void InitEngine()
{
	int	a;

	for(a=0;a<80;a++){ 
		Rot[a].v1=0.0;
		Rot[a].v2=0.0;
		Rot[a].v3=0.0;
	}
	o_idx=0;
	fx=((float)(SCREEN_WIDTH))/640.0;
	fy=((float)(SCREEN_HEIGHT))/480.0;
	maxx=SCREEN_WIDTH-2;
	maxy=SCREEN_HEIGHT-2;
	centerx=SCREEN_WIDTH/2.0;
	centery=SCREEN_HEIGHT/2.0;
}

void UpdateRot(){
	int	a;

	for(a=0;a<o_idx;a++){
		Objects[a].Rotation.v1=Rot[a].v1;
		Objects[a].Rotation.v2=Rot[a].v2;
		Objects[a].Rotation.v3=Rot[a].v3;
	}
}

void UpdateMov(){
	int	a;

	for(a=0;a<o_idx;a++){
		Objects[a].Position.v1=Mov[a].v1;
		Objects[a].Position.v2=Mov[a].v2;
		Objects[a].Position.v3=Mov[a].v3;
	}
}

void LoadMov(){
	int	a;

	for(a=0;a<o_idx;a++){
		Mov[a].v1=Objects[a].Position.v1;
		Mov[a].v2=Objects[a].Position.v2;
		Mov[a].v3=Objects[a].Position.v3;
	}
}

void	UnitVector(VECTOR *r1,VECTOR *r)
{
	float	a;
	a=fsqrt(r1->v1*r1->v1+r1->v2*r1->v2+r1->v3*r1->v3);
	if(a==0.0){
		r->v1=0.0;
		r->v2=0.0;
		r->v3=0.0;		
	} else {
		r->v1=r1->v1/a;
		r->v2=r1->v2/a;
		r->v3=r1->v3/a;
	}
}
void	FlatColor(FACE3D *f)
{
	VECTOR	a,b;
	VECTOR	Normal;

	a.v1=f->Vt[1].v1-f->Vt[0].v1;
	a.v2=f->Vt[1].v2-f->Vt[0].v2;
	a.v3=f->Vt[1].v3-f->Vt[0].v3;

	b.v1=f->Vt[3].v1-f->Vt[0].v1;
	b.v2=f->Vt[3].v2-f->Vt[0].v2;
	b.v3=f->Vt[3].v3-f->Vt[0].v3;

	Normal.v1=(a.v2*b.v3)-(a.v3*b.v2);
	Normal.v2=(a.v3*b.v1)-(a.v1*b.v3);
	Normal.v3=(a.v1*b.v2)-(a.v2*b.v1);
	UnitVector(&Normal,&Normal);
	if(Normal.v3<0.0) f->Color=-Normal.v3*64;
	else f->Color=Normal.v3*64;
}

void	CreateCubeWPrimperan(VECTOR a,float r,int c)
{
	Objects[o_idx].DrawFace=DrawWireQuadPrimperan;
	Objects[o_idx].ComputeColor=NULL;
	Objects[o_idx].Position=a;
	Objects[o_idx].Color=c;
	Objects[o_idx].Rotation.v1=0.0;
	Objects[o_idx].Rotation.v2=0.0;
	Objects[o_idx].Rotation.v3=0.0;
	Objects[o_idx].NFaces=4;
	Objects[o_idx].Faces[0].Vertex[0].v1=r;
	Objects[o_idx].Faces[0].Vertex[0].v2=-r;
	Objects[o_idx].Faces[0].Vertex[0].v3=-r;
	Objects[o_idx].Faces[0].Vertex[1].v1=-r;
	Objects[o_idx].Faces[0].Vertex[1].v2=-r;
	Objects[o_idx].Faces[0].Vertex[1].v3=-r;
	Objects[o_idx].Faces[0].Vertex[2].v1=-r;
	Objects[o_idx].Faces[0].Vertex[2].v2=r;
	Objects[o_idx].Faces[0].Vertex[2].v3=-r;
	Objects[o_idx].Faces[0].Vertex[3].v1=r;
	Objects[o_idx].Faces[0].Vertex[3].v2=r;
	Objects[o_idx].Faces[0].Vertex[3].v3=-r;
	Objects[o_idx].Faces[1].Vertex[0].v1=r;
	Objects[o_idx].Faces[1].Vertex[0].v2=-r;
	Objects[o_idx].Faces[1].Vertex[0].v3=r;
	Objects[o_idx].Faces[1].Vertex[1].v1=r;
	Objects[o_idx].Faces[1].Vertex[1].v2=-r;
	Objects[o_idx].Faces[1].Vertex[1].v3=-r;
	Objects[o_idx].Faces[1].Vertex[2].v1=r;
	Objects[o_idx].Faces[1].Vertex[2].v2=r;
	Objects[o_idx].Faces[1].Vertex[2].v3=-r;
	Objects[o_idx].Faces[1].Vertex[3].v1=r;
	Objects[o_idx].Faces[1].Vertex[3].v2=r;
	Objects[o_idx].Faces[1].Vertex[3].v3=r;
	Objects[o_idx].Faces[2].Vertex[0].v1=-r;
	Objects[o_idx].Faces[2].Vertex[0].v2=r;
	Objects[o_idx].Faces[2].Vertex[0].v3=r;
	Objects[o_idx].Faces[2].Vertex[1].v1=r;
	Objects[o_idx].Faces[2].Vertex[1].v2=r;
	Objects[o_idx].Faces[2].Vertex[1].v3=r;
	Objects[o_idx].Faces[2].Vertex[2].v1=r;
	Objects[o_idx].Faces[2].Vertex[2].v2=-r;
	Objects[o_idx].Faces[2].Vertex[2].v3=r;
	Objects[o_idx].Faces[2].Vertex[3].v1=-r;
	Objects[o_idx].Faces[2].Vertex[3].v2=-r;
	Objects[o_idx].Faces[2].Vertex[3].v3=r;
	Objects[o_idx].Faces[3].Vertex[0].v1=-r;
	Objects[o_idx].Faces[3].Vertex[0].v2=-r;
	Objects[o_idx].Faces[3].Vertex[0].v3=-r;
	Objects[o_idx].Faces[3].Vertex[1].v1=-r;
	Objects[o_idx].Faces[3].Vertex[1].v2=-r;
	Objects[o_idx].Faces[3].Vertex[1].v3=r;
	Objects[o_idx].Faces[3].Vertex[2].v1=-r;
	Objects[o_idx].Faces[3].Vertex[2].v2=r;
	Objects[o_idx].Faces[3].Vertex[2].v3=r;
	Objects[o_idx].Faces[3].Vertex[3].v1=-r;
	Objects[o_idx].Faces[3].Vertex[3].v2=r;
	Objects[o_idx].Faces[3].Vertex[3].v3=-r;

	o_idx++;
}


void	CreateCubeW(VECTOR a,float r,int c)
{
	Objects[o_idx].DrawFace=DrawWireQuad;
	Objects[o_idx].ComputeColor=NULL;
	Objects[o_idx].Position=a;
	Objects[o_idx].Color=c;
	Objects[o_idx].Rotation.v1=0.0;
	Objects[o_idx].Rotation.v2=0.0;
	Objects[o_idx].Rotation.v3=0.0;
	Objects[o_idx].NFaces=4;
	Objects[o_idx].Faces[0].Vertex[0].v1=r;
	Objects[o_idx].Faces[0].Vertex[0].v2=-r;
	Objects[o_idx].Faces[0].Vertex[0].v3=-r;
	Objects[o_idx].Faces[0].Vertex[1].v1=-r;
	Objects[o_idx].Faces[0].Vertex[1].v2=-r;
	Objects[o_idx].Faces[0].Vertex[1].v3=-r;
	Objects[o_idx].Faces[0].Vertex[2].v1=-r;
	Objects[o_idx].Faces[0].Vertex[2].v2=r;
	Objects[o_idx].Faces[0].Vertex[2].v3=-r;
	Objects[o_idx].Faces[0].Vertex[3].v1=r;
	Objects[o_idx].Faces[0].Vertex[3].v2=r;
	Objects[o_idx].Faces[0].Vertex[3].v3=-r;
	Objects[o_idx].Faces[1].Vertex[0].v1=r;
	Objects[o_idx].Faces[1].Vertex[0].v2=-r;
	Objects[o_idx].Faces[1].Vertex[0].v3=r;
	Objects[o_idx].Faces[1].Vertex[1].v1=r;
	Objects[o_idx].Faces[1].Vertex[1].v2=-r;
	Objects[o_idx].Faces[1].Vertex[1].v3=-r;
	Objects[o_idx].Faces[1].Vertex[2].v1=r;
	Objects[o_idx].Faces[1].Vertex[2].v2=r;
	Objects[o_idx].Faces[1].Vertex[2].v3=-r;
	Objects[o_idx].Faces[1].Vertex[3].v1=r;
	Objects[o_idx].Faces[1].Vertex[3].v2=r;
	Objects[o_idx].Faces[1].Vertex[3].v3=r;
	Objects[o_idx].Faces[2].Vertex[0].v1=-r;
	Objects[o_idx].Faces[2].Vertex[0].v2=r;
	Objects[o_idx].Faces[2].Vertex[0].v3=r;
	Objects[o_idx].Faces[2].Vertex[1].v1=r;
	Objects[o_idx].Faces[2].Vertex[1].v2=r;
	Objects[o_idx].Faces[2].Vertex[1].v3=r;
	Objects[o_idx].Faces[2].Vertex[2].v1=r;
	Objects[o_idx].Faces[2].Vertex[2].v2=-r;
	Objects[o_idx].Faces[2].Vertex[2].v3=r;
	Objects[o_idx].Faces[2].Vertex[3].v1=-r;
	Objects[o_idx].Faces[2].Vertex[3].v2=-r;
	Objects[o_idx].Faces[2].Vertex[3].v3=r;
	Objects[o_idx].Faces[3].Vertex[0].v1=-r;
	Objects[o_idx].Faces[3].Vertex[0].v2=-r;
	Objects[o_idx].Faces[3].Vertex[0].v3=-r;
	Objects[o_idx].Faces[3].Vertex[1].v1=-r;
	Objects[o_idx].Faces[3].Vertex[1].v2=-r;
	Objects[o_idx].Faces[3].Vertex[1].v3=r;
	Objects[o_idx].Faces[3].Vertex[2].v1=-r;
	Objects[o_idx].Faces[3].Vertex[2].v2=r;
	Objects[o_idx].Faces[3].Vertex[2].v3=r;
	Objects[o_idx].Faces[3].Vertex[3].v1=-r;
	Objects[o_idx].Faces[3].Vertex[3].v2=r;
	Objects[o_idx].Faces[3].Vertex[3].v3=-r;

	o_idx++;
}


void	CreateCubeTri(VECTOR a,float r,int c)
{
	Objects[o_idx].DrawFace=DrawFlatAdd;//DrawWire;
	Objects[o_idx].ComputeColor=NULL;
	Objects[o_idx].Position=a;
	Objects[o_idx].Color=c;
	Objects[o_idx].Rotation.v1=0.0;
	Objects[o_idx].Rotation.v2=0.0;
	Objects[o_idx].Rotation.v3=0.0;
	Objects[o_idx].NFaces=10;	//12;

	#warning ccm was here, fucking code

	Objects[o_idx].Faces[0].Vertex[0].v1=-r;
	Objects[o_idx].Faces[0].Vertex[0].v2=-r;
	Objects[o_idx].Faces[0].Vertex[0].v3=-r;
	Objects[o_idx].Faces[0].Vertex[1].v1=-r;
	Objects[o_idx].Faces[0].Vertex[1].v2=r;
	Objects[o_idx].Faces[0].Vertex[1].v3=-r;
	Objects[o_idx].Faces[0].Vertex[2].v1=r;
	Objects[o_idx].Faces[0].Vertex[2].v2=r;
	Objects[o_idx].Faces[0].Vertex[2].v3=-r;
	Objects[o_idx].Faces[0].Vertex[3].v1=+r;
	Objects[o_idx].Faces[0].Vertex[3].v2=-r;
	Objects[o_idx].Faces[0].Vertex[3].v3=-r;
	Objects[o_idx].Faces[1].Vertex[0].v1=r;
	Objects[o_idx].Faces[1].Vertex[0].v2=r;
	Objects[o_idx].Faces[1].Vertex[0].v3=-r;
	Objects[o_idx].Faces[1].Vertex[1].v1=-r;
	Objects[o_idx].Faces[1].Vertex[1].v2=-r;
	Objects[o_idx].Faces[1].Vertex[1].v3=-r;
	Objects[o_idx].Faces[1].Vertex[2].v1=r;
	Objects[o_idx].Faces[1].Vertex[2].v2=-r;
	Objects[o_idx].Faces[1].Vertex[2].v3=-r;
	Objects[o_idx].Faces[1].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[1].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[1].Vertex[3].v3=0.0;

	Objects[o_idx].Faces[2].Vertex[0].v1=r;
	Objects[o_idx].Faces[2].Vertex[0].v2=-r;
	Objects[o_idx].Faces[2].Vertex[0].v3=-r;
	Objects[o_idx].Faces[2].Vertex[1].v1=r;
	Objects[o_idx].Faces[2].Vertex[1].v2=r;
	Objects[o_idx].Faces[2].Vertex[1].v3=-r;
	Objects[o_idx].Faces[2].Vertex[2].v1=r;
	Objects[o_idx].Faces[2].Vertex[2].v2=r;
	Objects[o_idx].Faces[2].Vertex[2].v3=r;
	Objects[o_idx].Faces[2].Vertex[3].v1=r;
	Objects[o_idx].Faces[2].Vertex[3].v2=-r;
	Objects[o_idx].Faces[2].Vertex[3].v3=r;
	Objects[o_idx].Faces[3].Vertex[0].v1=r;
	Objects[o_idx].Faces[3].Vertex[0].v2=r;
	Objects[o_idx].Faces[3].Vertex[0].v3=r;
	Objects[o_idx].Faces[3].Vertex[1].v1=r;
	Objects[o_idx].Faces[3].Vertex[1].v2=-r;
	Objects[o_idx].Faces[3].Vertex[1].v3=-r;
	Objects[o_idx].Faces[3].Vertex[2].v1=r;
	Objects[o_idx].Faces[3].Vertex[2].v2=-r;
	Objects[o_idx].Faces[3].Vertex[2].v3=r;
	Objects[o_idx].Faces[3].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[3].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[3].Vertex[3].v3=0.0;

	Objects[o_idx].Faces[4].Vertex[0].v1=-r;
	Objects[o_idx].Faces[4].Vertex[0].v2=-r;
	Objects[o_idx].Faces[4].Vertex[0].v3=-r;
	Objects[o_idx].Faces[4].Vertex[1].v1=-r;
	Objects[o_idx].Faces[4].Vertex[1].v2=-r;
	Objects[o_idx].Faces[4].Vertex[1].v3=r;
	Objects[o_idx].Faces[4].Vertex[2].v1=r;
	Objects[o_idx].Faces[4].Vertex[2].v2=-r;
	Objects[o_idx].Faces[4].Vertex[2].v3=r;
	Objects[o_idx].Faces[4].Vertex[3].v1=r;
	Objects[o_idx].Faces[4].Vertex[3].v2=-r;
	Objects[o_idx].Faces[4].Vertex[3].v3=-r;
	Objects[o_idx].Faces[5].Vertex[0].v1=-r;
	Objects[o_idx].Faces[5].Vertex[0].v2=-r;
	Objects[o_idx].Faces[5].Vertex[0].v3=-r;
	Objects[o_idx].Faces[5].Vertex[1].v1=r;
	Objects[o_idx].Faces[5].Vertex[1].v2=-r;
	Objects[o_idx].Faces[5].Vertex[1].v3=-r;
	Objects[o_idx].Faces[5].Vertex[2].v1=r;
	Objects[o_idx].Faces[5].Vertex[2].v2=-r;
	Objects[o_idx].Faces[5].Vertex[2].v3=r;
	Objects[o_idx].Faces[5].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[5].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[5].Vertex[3].v3=0.0;

	Objects[o_idx].Faces[6].Vertex[0].v1=-r;
	Objects[o_idx].Faces[6].Vertex[0].v2=r;
	Objects[o_idx].Faces[6].Vertex[0].v3=-r;
	Objects[o_idx].Faces[6].Vertex[1].v1=-r;
	Objects[o_idx].Faces[6].Vertex[1].v2=r;
	Objects[o_idx].Faces[6].Vertex[1].v3=r;
	Objects[o_idx].Faces[6].Vertex[2].v1=r;
	Objects[o_idx].Faces[6].Vertex[2].v2=r;
	Objects[o_idx].Faces[6].Vertex[2].v3=r;
	Objects[o_idx].Faces[6].Vertex[3].v1=r;
	Objects[o_idx].Faces[6].Vertex[3].v2=r;
	Objects[o_idx].Faces[6].Vertex[3].v3=-r;
	Objects[o_idx].Faces[7].Vertex[0].v1=-r;
	Objects[o_idx].Faces[7].Vertex[0].v2=r;
	Objects[o_idx].Faces[7].Vertex[0].v3=-r;
	Objects[o_idx].Faces[7].Vertex[1].v1=r;
	Objects[o_idx].Faces[7].Vertex[1].v2=r;
	Objects[o_idx].Faces[7].Vertex[1].v3=-r;
	Objects[o_idx].Faces[7].Vertex[2].v1=r;
	Objects[o_idx].Faces[7].Vertex[2].v2=r;
	Objects[o_idx].Faces[7].Vertex[2].v3=r;
	Objects[o_idx].Faces[7].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[7].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[7].Vertex[3].v3=0.0;

	Objects[o_idx].Faces[8].Vertex[0].v1=-r;
	Objects[o_idx].Faces[8].Vertex[0].v2=-r;
	Objects[o_idx].Faces[8].Vertex[0].v3=-r;
	Objects[o_idx].Faces[8].Vertex[1].v1=-r;
	Objects[o_idx].Faces[8].Vertex[1].v2=r;
	Objects[o_idx].Faces[8].Vertex[1].v3=-r;
	Objects[o_idx].Faces[8].Vertex[2].v1=-r;
	Objects[o_idx].Faces[8].Vertex[2].v2=r;
	Objects[o_idx].Faces[8].Vertex[2].v3=r;
	Objects[o_idx].Faces[8].Vertex[3].v1=-r;
	Objects[o_idx].Faces[8].Vertex[3].v2=-r;
	Objects[o_idx].Faces[8].Vertex[3].v3=r;
	Objects[o_idx].Faces[9].Vertex[0].v1=-r;
	Objects[o_idx].Faces[9].Vertex[0].v2=r;
	Objects[o_idx].Faces[9].Vertex[0].v3=r;
	Objects[o_idx].Faces[9].Vertex[1].v1=-r;
	Objects[o_idx].Faces[9].Vertex[1].v2=-r;
	Objects[o_idx].Faces[9].Vertex[1].v3=-r;
	Objects[o_idx].Faces[9].Vertex[2].v1=-r;
	Objects[o_idx].Faces[9].Vertex[2].v2=-r;
	Objects[o_idx].Faces[9].Vertex[2].v3=r;
	Objects[o_idx].Faces[9].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[9].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[9].Vertex[3].v3=0.0;

	o_idx++;
}

void	CreateCubeTriFlat(VECTOR a,float r,int c)
{
	CreateCubeTri(a,r,c);
	Objects[o_idx-1].DrawFace=DrawFlat;
	Objects[o_idx-1].ComputeColor=FlatColor;

/*
	Objects[o_idx].DrawFace=DrawFlat;//DrawWire;
	Objects[o_idx].ComputeColor=FlatColor;
	Objects[o_idx].Position=a;
	Objects[o_idx].Color=c;
	Objects[o_idx].Rotation.v1=0.0;
	Objects[o_idx].Rotation.v2=0.0;
	Objects[o_idx].Rotation.v3=0.0;
	Objects[o_idx].NFaces=10;	//12;
	Objects[o_idx].Faces[0].Vertex[0].v1=-r;
	Objects[o_idx].Faces[0].Vertex[0].v2=-r;
	Objects[o_idx].Faces[0].Vertex[0].v3=-r;
	Objects[o_idx].Faces[0].Vertex[1].v1=-r;
	Objects[o_idx].Faces[0].Vertex[1].v2=r;
	Objects[o_idx].Faces[0].Vertex[1].v3=-r;
	Objects[o_idx].Faces[0].Vertex[2].v1=r;
	Objects[o_idx].Faces[0].Vertex[2].v2=r;
	Objects[o_idx].Faces[0].Vertex[2].v3=-r;
	Objects[o_idx].Faces[0].Vertex[3].v1=r;
	Objects[o_idx].Faces[0].Vertex[3].v2=-r;
	Objects[o_idx].Faces[0].Vertex[3].v3=-r;
	Objects[o_idx].Faces[1].Vertex[0].v1=r;
	Objects[o_idx].Faces[1].Vertex[0].v2=r;
	Objects[o_idx].Faces[1].Vertex[0].v3=-r;
	Objects[o_idx].Faces[1].Vertex[1].v1=-r;
	Objects[o_idx].Faces[1].Vertex[1].v2=-r;
	Objects[o_idx].Faces[1].Vertex[1].v3=-r;
	Objects[o_idx].Faces[1].Vertex[2].v1=r;
	Objects[o_idx].Faces[1].Vertex[2].v2=-r;
	Objects[o_idx].Faces[1].Vertex[2].v3=-r;
	Objects[o_idx].Faces[1].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[1].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[1].Vertex[3].v3=0.0;

	Objects[o_idx].Faces[2].Vertex[0].v1=r;
	Objects[o_idx].Faces[2].Vertex[0].v2=-r;
	Objects[o_idx].Faces[2].Vertex[0].v3=-r;
	Objects[o_idx].Faces[2].Vertex[1].v1=r;
	Objects[o_idx].Faces[2].Vertex[1].v2=r;
	Objects[o_idx].Faces[2].Vertex[1].v3=-r;
	Objects[o_idx].Faces[2].Vertex[2].v1=r;
	Objects[o_idx].Faces[2].Vertex[2].v2=r;
	Objects[o_idx].Faces[2].Vertex[2].v3=r;
	Objects[o_idx].Faces[2].Vertex[3].v1=r;
	Objects[o_idx].Faces[2].Vertex[3].v2=-r;
	Objects[o_idx].Faces[2].Vertex[3].v3=r;
	Objects[o_idx].Faces[3].Vertex[0].v1=r;
	Objects[o_idx].Faces[3].Vertex[0].v2=r;
	Objects[o_idx].Faces[3].Vertex[0].v3=r;
	Objects[o_idx].Faces[3].Vertex[1].v1=r;
	Objects[o_idx].Faces[3].Vertex[1].v2=-r;
	Objects[o_idx].Faces[3].Vertex[1].v3=-r;
	Objects[o_idx].Faces[3].Vertex[2].v1=r;
	Objects[o_idx].Faces[3].Vertex[2].v2=-r;
	Objects[o_idx].Faces[3].Vertex[2].v3=r;
	Objects[o_idx].Faces[3].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[3].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[3].Vertex[3].v3=0.0;

	Objects[o_idx].Faces[4].Vertex[0].v1=-r;
	Objects[o_idx].Faces[4].Vertex[0].v2=-r;
	Objects[o_idx].Faces[4].Vertex[0].v3=-r;
	Objects[o_idx].Faces[4].Vertex[1].v1=-r;
	Objects[o_idx].Faces[4].Vertex[1].v2=-r;
	Objects[o_idx].Faces[4].Vertex[1].v3=r;
	Objects[o_idx].Faces[4].Vertex[2].v1=r;
	Objects[o_idx].Faces[4].Vertex[2].v2=-r;
	Objects[o_idx].Faces[4].Vertex[2].v3=r;
	Objects[o_idx].Faces[4].Vertex[3].v1=r;
	Objects[o_idx].Faces[4].Vertex[3].v2=-r;
	Objects[o_idx].Faces[4].Vertex[3].v3=-r;
	Objects[o_idx].Faces[5].Vertex[0].v1=-r;
	Objects[o_idx].Faces[5].Vertex[0].v2=-r;
	Objects[o_idx].Faces[5].Vertex[0].v3=-r;
	Objects[o_idx].Faces[5].Vertex[1].v1=r;
	Objects[o_idx].Faces[5].Vertex[1].v2=-r;
	Objects[o_idx].Faces[5].Vertex[1].v3=-r;
	Objects[o_idx].Faces[5].Vertex[2].v1=r;
	Objects[o_idx].Faces[5].Vertex[2].v2=-r;
	Objects[o_idx].Faces[5].Vertex[2].v3=r;
	Objects[o_idx].Faces[5].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[5].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[5].Vertex[3].v3=0.0;

	Objects[o_idx].Faces[6].Vertex[0].v1=-r;
	Objects[o_idx].Faces[6].Vertex[0].v2=r;
	Objects[o_idx].Faces[6].Vertex[0].v3=-r;
	Objects[o_idx].Faces[6].Vertex[1].v1=-r;
	Objects[o_idx].Faces[6].Vertex[1].v2=r;
	Objects[o_idx].Faces[6].Vertex[1].v3=r;
	Objects[o_idx].Faces[6].Vertex[2].v1=r;
	Objects[o_idx].Faces[6].Vertex[2].v2=r;
	Objects[o_idx].Faces[6].Vertex[2].v3=r;
	Objects[o_idx].Faces[6].Vertex[3].v1=r;
	Objects[o_idx].Faces[6].Vertex[3].v2=r;
	Objects[o_idx].Faces[6].Vertex[3].v3=-r;
	Objects[o_idx].Faces[7].Vertex[0].v1=-r;
	Objects[o_idx].Faces[7].Vertex[0].v2=r;
	Objects[o_idx].Faces[7].Vertex[0].v3=-r;
	Objects[o_idx].Faces[7].Vertex[1].v1=r;
	Objects[o_idx].Faces[7].Vertex[1].v2=r;
	Objects[o_idx].Faces[7].Vertex[1].v3=-r;
	Objects[o_idx].Faces[7].Vertex[2].v1=r;
	Objects[o_idx].Faces[7].Vertex[2].v2=r;
	Objects[o_idx].Faces[7].Vertex[2].v3=r;
	Objects[o_idx].Faces[7].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[7].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[7].Vertex[3].v3=0.0;

	Objects[o_idx].Faces[8].Vertex[0].v1=-r;
	Objects[o_idx].Faces[8].Vertex[0].v2=-r;
	Objects[o_idx].Faces[8].Vertex[0].v3=-r;
	Objects[o_idx].Faces[8].Vertex[1].v1=-r;
	Objects[o_idx].Faces[8].Vertex[1].v2=r;
	Objects[o_idx].Faces[8].Vertex[1].v3=-r;
	Objects[o_idx].Faces[8].Vertex[2].v1=-r;
	Objects[o_idx].Faces[8].Vertex[2].v2=r;
	Objects[o_idx].Faces[8].Vertex[2].v3=r;
	Objects[o_idx].Faces[8].Vertex[3].v1=-r;
	Objects[o_idx].Faces[8].Vertex[3].v2=-r;
	Objects[o_idx].Faces[8].Vertex[3].v3=r;
	Objects[o_idx].Faces[9].Vertex[0].v1=-r;
	Objects[o_idx].Faces[9].Vertex[0].v2=r;
	Objects[o_idx].Faces[9].Vertex[0].v3=r;
	Objects[o_idx].Faces[9].Vertex[1].v1=-r;
	Objects[o_idx].Faces[9].Vertex[1].v2=-r;
	Objects[o_idx].Faces[9].Vertex[1].v3=-r;
	Objects[o_idx].Faces[9].Vertex[2].v1=-r;
	Objects[o_idx].Faces[9].Vertex[2].v2=-r;
	Objects[o_idx].Faces[9].Vertex[2].v3=r;
	Objects[o_idx].Faces[9].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[9].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[9].Vertex[3].v3=0.0;

	o_idx++;
*/
}

void	CreateCubeTriFlat2(VECTOR a,float r,int c)
{
	Objects[o_idx].DrawFace=DrawFlatAdd;//DrawWire;
	Objects[o_idx].ComputeColor=NULL;	//FlatColor;
	Objects[o_idx].Position.v1=0.0;
	Objects[o_idx].Position.v2=0.0;
	Objects[o_idx].Position.v3=0.0;
	Objects[o_idx].Color=c;
	Objects[o_idx].Rotation.v1=0.0;
	Objects[o_idx].Rotation.v2=0.0;
	Objects[o_idx].Rotation.v3=0.0;
	Objects[o_idx].NFaces=10;	//12;
	Objects[o_idx].Faces[0].Vertex[0].v1=a.v1-r;
	Objects[o_idx].Faces[0].Vertex[0].v2=a.v2-r;
	Objects[o_idx].Faces[0].Vertex[0].v3=a.v3-r*2;
	Objects[o_idx].Faces[0].Vertex[1].v1=a.v1-r;
	Objects[o_idx].Faces[0].Vertex[1].v2=a.v2+r;
	Objects[o_idx].Faces[0].Vertex[1].v3=a.v3-r*2;
	Objects[o_idx].Faces[0].Vertex[2].v1=a.v1+r;
	Objects[o_idx].Faces[0].Vertex[2].v2=a.v2+r;
	Objects[o_idx].Faces[0].Vertex[2].v3=a.v3-r*2;
	Objects[o_idx].Faces[0].Vertex[3].v1=a.v1+r;
	Objects[o_idx].Faces[0].Vertex[3].v2=a.v2-r;
	Objects[o_idx].Faces[0].Vertex[3].v3=a.v3-r*2;
	Objects[o_idx].Faces[1].Vertex[0].v1=a.v1+r;
	Objects[o_idx].Faces[1].Vertex[0].v2=a.v2+r;
	Objects[o_idx].Faces[1].Vertex[0].v3=a.v3-r*2;
	Objects[o_idx].Faces[1].Vertex[1].v1=a.v1-r;
	Objects[o_idx].Faces[1].Vertex[1].v2=a.v2-r;
	Objects[o_idx].Faces[1].Vertex[1].v3=a.v3-r*2;
	Objects[o_idx].Faces[1].Vertex[2].v1=a.v1+r;
	Objects[o_idx].Faces[1].Vertex[2].v2=a.v2-r;
	Objects[o_idx].Faces[1].Vertex[2].v3=a.v3-r*2;
	Objects[o_idx].Faces[1].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[1].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[1].Vertex[3].v3=0.0;

	Objects[o_idx].Faces[2].Vertex[0].v1=a.v1+r;
	Objects[o_idx].Faces[2].Vertex[0].v2=a.v2-r;
	Objects[o_idx].Faces[2].Vertex[0].v3=a.v3-r*2;
	Objects[o_idx].Faces[2].Vertex[1].v1=a.v1+r;
	Objects[o_idx].Faces[2].Vertex[1].v2=a.v2+r;
	Objects[o_idx].Faces[2].Vertex[1].v3=a.v3-r*2;
	Objects[o_idx].Faces[2].Vertex[2].v1=a.v1+r;
	Objects[o_idx].Faces[2].Vertex[2].v2=a.v2+r;
	Objects[o_idx].Faces[2].Vertex[2].v3=a.v3+r*2;
	Objects[o_idx].Faces[2].Vertex[3].v1=a.v1+r;
	Objects[o_idx].Faces[2].Vertex[3].v2=a.v2-r;
	Objects[o_idx].Faces[2].Vertex[3].v3=a.v3+r*2;
	Objects[o_idx].Faces[3].Vertex[0].v1=a.v1+r;
	Objects[o_idx].Faces[3].Vertex[0].v2=a.v2+r;
	Objects[o_idx].Faces[3].Vertex[0].v3=a.v3+r*2;
	Objects[o_idx].Faces[3].Vertex[1].v1=a.v1+r;
	Objects[o_idx].Faces[3].Vertex[1].v2=a.v2-r;
	Objects[o_idx].Faces[3].Vertex[1].v3=a.v3-r*2;
	Objects[o_idx].Faces[3].Vertex[2].v1=a.v1+r;
	Objects[o_idx].Faces[3].Vertex[2].v2=a.v2-r;
	Objects[o_idx].Faces[3].Vertex[2].v3=a.v3+r*2;
	Objects[o_idx].Faces[3].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[3].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[3].Vertex[3].v3=0.0;

	Objects[o_idx].Faces[4].Vertex[0].v1=a.v1-r;
	Objects[o_idx].Faces[4].Vertex[0].v2=a.v2-r;
	Objects[o_idx].Faces[4].Vertex[0].v3=a.v3-r*2;
	Objects[o_idx].Faces[4].Vertex[1].v1=a.v1-r;
	Objects[o_idx].Faces[4].Vertex[1].v2=a.v2-r;
	Objects[o_idx].Faces[4].Vertex[1].v3=a.v3+r*2;
	Objects[o_idx].Faces[4].Vertex[2].v1=a.v1+r;
	Objects[o_idx].Faces[4].Vertex[2].v2=a.v2-r;
	Objects[o_idx].Faces[4].Vertex[2].v3=a.v3+r*2;
	Objects[o_idx].Faces[4].Vertex[3].v1=a.v1+r;
	Objects[o_idx].Faces[4].Vertex[3].v2=a.v2-r;
	Objects[o_idx].Faces[4].Vertex[3].v3=a.v3-r*2;
	Objects[o_idx].Faces[5].Vertex[0].v1=a.v1-r;
	Objects[o_idx].Faces[5].Vertex[0].v2=a.v2-r;
	Objects[o_idx].Faces[5].Vertex[0].v3=a.v3-r*2;
	Objects[o_idx].Faces[5].Vertex[1].v1=a.v1+r;
	Objects[o_idx].Faces[5].Vertex[1].v2=a.v2-r;
	Objects[o_idx].Faces[5].Vertex[1].v3=a.v3-r*2;
	Objects[o_idx].Faces[5].Vertex[2].v1=a.v1+r;
	Objects[o_idx].Faces[5].Vertex[2].v2=a.v2-r;
	Objects[o_idx].Faces[5].Vertex[2].v3=a.v3+r*2;
	Objects[o_idx].Faces[5].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[5].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[5].Vertex[3].v3=0.0;

	Objects[o_idx].Faces[6].Vertex[0].v1=a.v1-r;
	Objects[o_idx].Faces[6].Vertex[0].v2=a.v2+r;
	Objects[o_idx].Faces[6].Vertex[0].v3=a.v3-r*2;
	Objects[o_idx].Faces[6].Vertex[1].v1=a.v1-r;
	Objects[o_idx].Faces[6].Vertex[1].v2=a.v2+r;
	Objects[o_idx].Faces[6].Vertex[1].v3=a.v3+r*2;
	Objects[o_idx].Faces[6].Vertex[2].v1=a.v1+r;
	Objects[o_idx].Faces[6].Vertex[2].v2=a.v2+r;
	Objects[o_idx].Faces[6].Vertex[2].v3=a.v3+r*2;
	Objects[o_idx].Faces[6].Vertex[3].v1=a.v1+r;
	Objects[o_idx].Faces[6].Vertex[3].v2=a.v2+r;
	Objects[o_idx].Faces[6].Vertex[3].v3=a.v3-r*2;
	Objects[o_idx].Faces[7].Vertex[0].v1=a.v1-r;
	Objects[o_idx].Faces[7].Vertex[0].v2=a.v2+r;
	Objects[o_idx].Faces[7].Vertex[0].v3=a.v3-r*2;
	Objects[o_idx].Faces[7].Vertex[1].v1=a.v1+r;
	Objects[o_idx].Faces[7].Vertex[1].v2=a.v2+r;
	Objects[o_idx].Faces[7].Vertex[1].v3=a.v3-r*2;
	Objects[o_idx].Faces[7].Vertex[2].v1=a.v1+r;
	Objects[o_idx].Faces[7].Vertex[2].v2=a.v2+r;
	Objects[o_idx].Faces[7].Vertex[2].v3=a.v3+r*2;
	Objects[o_idx].Faces[7].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[7].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[7].Vertex[3].v3=0.0;

	Objects[o_idx].Faces[8].Vertex[0].v1=a.v1-r;
	Objects[o_idx].Faces[8].Vertex[0].v2=a.v2-r;
	Objects[o_idx].Faces[8].Vertex[0].v3=a.v3-r*2;
	Objects[o_idx].Faces[8].Vertex[1].v1=a.v1-r;
	Objects[o_idx].Faces[8].Vertex[1].v2=a.v2+r;
	Objects[o_idx].Faces[8].Vertex[1].v3=a.v3-r*2;
	Objects[o_idx].Faces[8].Vertex[2].v1=a.v1-r;
	Objects[o_idx].Faces[8].Vertex[2].v2=a.v2+r;
	Objects[o_idx].Faces[8].Vertex[2].v3=a.v3+r*2;
	Objects[o_idx].Faces[8].Vertex[3].v1=a.v1-r;
	Objects[o_idx].Faces[8].Vertex[3].v2=a.v2-r;
	Objects[o_idx].Faces[8].Vertex[3].v3=a.v3+r*2;
	Objects[o_idx].Faces[9].Vertex[0].v1=a.v1-r;
	Objects[o_idx].Faces[9].Vertex[0].v2=a.v2+r;
	Objects[o_idx].Faces[9].Vertex[0].v3=a.v3+r*2;
	Objects[o_idx].Faces[9].Vertex[1].v1=a.v1-r;
	Objects[o_idx].Faces[9].Vertex[1].v2=a.v2-r;
	Objects[o_idx].Faces[9].Vertex[1].v3=a.v3-r*2;
	Objects[o_idx].Faces[9].Vertex[2].v1=a.v1-r;
	Objects[o_idx].Faces[9].Vertex[2].v2=a.v2-r;
	Objects[o_idx].Faces[9].Vertex[2].v3=a.v3+r*2;
	Objects[o_idx].Faces[9].Vertex[3].v1=0.0;
	Objects[o_idx].Faces[9].Vertex[3].v2=0.0;
	Objects[o_idx].Faces[9].Vertex[3].v3=0.0;

	o_idx++;
}

void	CreateCubeWrel(VECTOR a,float r,int c)
{
	Objects[o_idx].DrawFace=DrawWireQuad;
	Objects[o_idx].ComputeColor=NULL;
	Objects[o_idx].Position.v1=0.0;
	Objects[o_idx].Position.v2=0.0;
	Objects[o_idx].Position.v3=0.0;
	Objects[o_idx].Color=c;
	Objects[o_idx].Rotation.v1=0.0;
	Objects[o_idx].Rotation.v2=0.0;
	Objects[o_idx].Rotation.v3=0.0;
	Objects[o_idx].NFaces=4;
	Objects[o_idx].Faces[0].Vertex[0].v1=a.v1+r;
	Objects[o_idx].Faces[0].Vertex[0].v2=a.v2-r;
	Objects[o_idx].Faces[0].Vertex[0].v3=a.v3-r;
	Objects[o_idx].Faces[0].Vertex[1].v1=a.v1-r;
	Objects[o_idx].Faces[0].Vertex[1].v2=a.v2-r;
	Objects[o_idx].Faces[0].Vertex[1].v3=a.v3-r;
	Objects[o_idx].Faces[0].Vertex[2].v1=a.v1-r;
	Objects[o_idx].Faces[0].Vertex[2].v2=a.v2+r;
	Objects[o_idx].Faces[0].Vertex[2].v3=a.v3-r;
	Objects[o_idx].Faces[0].Vertex[3].v1=a.v1+r;
	Objects[o_idx].Faces[0].Vertex[3].v2=a.v2+r;
	Objects[o_idx].Faces[0].Vertex[3].v3=a.v3-r;
	Objects[o_idx].Faces[1].Vertex[0].v1=a.v1+r;
	Objects[o_idx].Faces[1].Vertex[0].v2=a.v2-r;
	Objects[o_idx].Faces[1].Vertex[0].v3=a.v3+r;
	Objects[o_idx].Faces[1].Vertex[1].v1=a.v1+r;
	Objects[o_idx].Faces[1].Vertex[1].v2=a.v2-r;
	Objects[o_idx].Faces[1].Vertex[1].v3=a.v3-r;
	Objects[o_idx].Faces[1].Vertex[2].v1=a.v1+r;
	Objects[o_idx].Faces[1].Vertex[2].v2=a.v2+r;
	Objects[o_idx].Faces[1].Vertex[2].v3=a.v3-r;
	Objects[o_idx].Faces[1].Vertex[3].v1=a.v1+r;
	Objects[o_idx].Faces[1].Vertex[3].v2=a.v2+r;
	Objects[o_idx].Faces[1].Vertex[3].v3=a.v3+r;
	Objects[o_idx].Faces[2].Vertex[0].v1=a.v1-r;
	Objects[o_idx].Faces[2].Vertex[0].v2=a.v2+r;
	Objects[o_idx].Faces[2].Vertex[0].v3=a.v3+r;
	Objects[o_idx].Faces[2].Vertex[1].v1=a.v1+r;
	Objects[o_idx].Faces[2].Vertex[1].v2=a.v2+r;
	Objects[o_idx].Faces[2].Vertex[1].v3=a.v3+r;
	Objects[o_idx].Faces[2].Vertex[2].v1=a.v1+r;
	Objects[o_idx].Faces[2].Vertex[2].v2=a.v2-r;
	Objects[o_idx].Faces[2].Vertex[2].v3=a.v3+r;
	Objects[o_idx].Faces[2].Vertex[3].v1=a.v1-r;
	Objects[o_idx].Faces[2].Vertex[3].v2=a.v2-r;
	Objects[o_idx].Faces[2].Vertex[3].v3=a.v3+r;
	Objects[o_idx].Faces[3].Vertex[0].v1=a.v1-r;
	Objects[o_idx].Faces[3].Vertex[0].v2=a.v2-r;
	Objects[o_idx].Faces[3].Vertex[0].v3=a.v3-r;
	Objects[o_idx].Faces[3].Vertex[1].v1=a.v1-r;
	Objects[o_idx].Faces[3].Vertex[1].v2=a.v2-r;
	Objects[o_idx].Faces[3].Vertex[1].v3=a.v3+r;
	Objects[o_idx].Faces[3].Vertex[2].v1=a.v1-r;
	Objects[o_idx].Faces[3].Vertex[2].v2=a.v2+r;
	Objects[o_idx].Faces[3].Vertex[2].v3=a.v3+r;
	Objects[o_idx].Faces[3].Vertex[3].v1=a.v1-r;
	Objects[o_idx].Faces[3].Vertex[3].v2=a.v2+r;
	Objects[o_idx].Faces[3].Vertex[3].v3=a.v3-r;

	o_idx++;
}


void Render(CAMERA *cam,unsigned char *dst)
{
	int	a,c;
	int	drawface;

	CalcTransformM(&cam->Rotation,&cam->Transform);

	for(a=0;a<o_idx;a++){
		CalcTransformM(&Objects[a].Rotation,&Objects[a].Transform);
		for(nf=0;nf<Objects[a].NFaces;nf++){
			if(nf%2==0) {if(Objects[a].ComputeColor!=NULL) Objects[a].ComputeColor(&Objects[a].Faces[nf]);}
			else Objects[a].Faces[nf].Color=Objects[a].Faces[nf-1].Color;

			drawface=1;
			for(c=0;c<4 && drawface==1;c++){
				VxM(&Objects[a].Faces[nf].Vertex[c],&Objects[a].Transform,&Objects[a].Faces[nf].Vt[c]);
				Objects[a].Faces[nf].Vt[c].v1+=Objects[a].Position.v1;
				Objects[a].Faces[nf].Vt[c].v2+=Objects[a].Position.v2;
				Objects[a].Faces[nf].Vt[c].v3+=Objects[a].Position.v3;

				VxM(&Objects[a].Faces[nf].Vt[c],&cam->Transform,&Objects[a].Faces[nf].Vt[c]);
				Objects[a].Faces[nf].Vt[c].v1-=cam->Position.v1;
				Objects[a].Faces[nf].Vt[c].v2-=cam->Position.v2;
				Objects[a].Faces[nf].Vt[c].v3-=cam->Position.v3;
		
				if(Objects[a].Faces[nf].Vt[c].v3>0.0){
					Objects[a].Faces[nf].Vp[c].v1=(fx*cam->FOV*Objects[a].Faces[nf].Vt[c].v1/Objects[a].Faces[nf].Vt[c].v3)+centerx;
					Objects[a].Faces[nf].Vp[c].v2=(fy*cam->FOV*Objects[a].Faces[nf].Vt[c].v2/Objects[a].Faces[nf].Vt[c].v3)+centery;
				} else {
					Objects[a].Faces[nf].Vp[c].v1=centerx;	//er?
					Objects[a].Faces[nf].Vp[c].v2=centery;	//er?
					drawface=0;
				}
			}
			if(drawface) {
				Objects[a].DrawFace(&Objects[a].Faces[nf],dst,Objects[a].Color);
			}
		}
	}
}


void	DrawCube(int x,int y,int size,unsigned char *dst){
	int	a,b;	//,ys1,ys2,xs1,xs2;
	int	pos;

	if(size>32) size=64-size;

	if((y+size/2)>(maxy+1) || (x+size/2)>(maxx+1) || (x-size/2)<0 || (y-size/2)<0) return;
	pos=(y-size/2)*SCREEN_WIDTH+x;
	for(a=0;a<size;a++){
		dst[pos-size/2]=20;
		dst[pos+size/2]=20;
		pos+=SCREEN_WIDTH;
	}

	b=size*SCREEN_WIDTH;
	pos=x-size/2+(y-size/2)*SCREEN_WIDTH;
	for(a=0;a<size;a++){
		dst[pos]=20;
		dst[pos+b]=20;
		pos++;
	}
}

