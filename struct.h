typedef struct vector VECTOR;
typedef struct matrix MATRIX;
typedef struct camera CAMERA;
typedef struct object OBJECT;
typedef struct face3d FACE3D;

typedef void (*DrawF) (FACE3D *,char *,int);
typedef void (*CompC) (FACE3D *);

struct vector{
	float	v1;
	float	v2;
	float	v3;
};


struct matrix{
	float 	m11,m12,m13;
	float 	m21,m22,m23;
	float 	m31,m32,m33;
};

struct camera{
	VECTOR	Position;
	VECTOR	Rotation;
	float	FOV;
	float	Roll;
	MATRIX	Transform;
	MATRIX	InvTrn;
	MATRIX	Final;
};

struct face3d{
	VECTOR	Vertex[4];		//Original
	VECTOR	Vt[4];			//Transformed
	VECTOR	Vp[4];			//Projected
	float	Color;
};

struct object{
	VECTOR	Position;
	VECTOR	Rotation;
	DrawF	DrawFace;
	CompC	ComputeColor;
	int	NFaces;
	FACE3D	Faces[15];	//ugh
	MATRIX	Transform;
	MATRIX	Final;
	int	Color;
};