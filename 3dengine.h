void	VxM(VECTOR *b,MATRIX *a,VECTOR *r);
void	CalcTransformM(VECTOR *Rot,MATRIX *Trans);
void 	InitEngine();

void	CreateCubeW(VECTOR a,float r,int c);
void	CreateCubeWPrimperan(VECTOR a,float r,int c);
void	CreateCubeTri(VECTOR a,float r,int c);
void	CreateCubeWrel(VECTOR a,float r,int c);
void	CreateCubeTriFlat(VECTOR a,float r,int c);
void	CreateCubeTriFlat2(VECTOR a,float r,int c);
void 	Line2(unsigned char *dst,float x1f,float y1f,float x2f,float y2f,int color);

void	inittable();

void 	Render(CAMERA *c,unsigned char *dst);
void 	RenderPrimperan(CAMERA *c,unsigned char *dst);
void	UpdateRot();
void	UpdateMov();
void	LoadMov();

void	DrawCube(int x,int y,int size, unsigned char *dst);
void	CubeScroller3(int fpos,int inip,int x1,int color,int dir,unsigned char *dst);

extern	VECTOR	Rot[100];
extern	VECTOR  Mov[100];
