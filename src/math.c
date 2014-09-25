#include "math.h"
#include "util.h"
#include <math.h>
#include <memory.h>

#define EPSINON_FLOAT 0.00001f
#define EPSINON_DOUBLE 0.0000001

static unsigned long power2[32];

void MathInit()
{
	int i=0,j=1;
	for(;i<32;i++)
	{
		power2[i]=j;
		j=j<<1;
	}
	LoggerInfo("Math library initialized");//数学库初始化不会失败...
}

unsigned long MathNextPower2( unsigned long v )
{
	int i=0;
	for(;i<32;i++)
	{
		if(power2[i]>=v)
		{
			return power2[i];
		}
	}
	return power2[31];
	/*return v<=2?2:
		v<=4?4:
		v<=8?8:
		v<=16?16:
		v<=32?32:
		v<=64?64:
		v<=128?128:
		v<=256?256:
		v<=512?512:
		v<=1024?1024:
		v<=2048?2048:
		v<=4096?4096:
		v<=8192?8192:
		v<=16384?16384:
		v<=32768?32768:
		v<=*/
}

BOOL MathFloatEqual( float a,float b )
{
	return fabs(a-b)<EPSINON_FLOAT;
}

BOOL MathDoubleEqual( double a,double b )
{
	return abs(a-b)<EPSINON_DOUBLE;
}

unsigned long MathNextMultiple8( unsigned long v )
{
	unsigned long i=v%8;
	return i==0?v:v+8-i;
}

Matrix MathMatrixCreate(Matrix* source)
{
	Matrix nMatrix;
	if(source!=NULL)
	{
		nMatrix = *source;
		//memcpy(&(nMatrix.m00),&(source->m00),16*sizeof(float));
	}
	else
	{
		MathMatrixLoadIdentity(&nMatrix);
	}
	return nMatrix;
}

void MathMatrixLoadIdentity(Matrix* matrix)
{
	memset(&(matrix->m00),0,16*sizeof(float));
	matrix->m00 = 1.0f;
	matrix->m11 = 1.0f;
	matrix->m22 = 1.0f;
	matrix->m33 = 1.0f;
	//matrix->m03 = 10.0f;
}

Matrix MathMatrixMultiplyMatrix(Matrix* left,Matrix* right)
{
	Matrix matrix;
	float temp00, temp01, temp02, temp03;
	float temp10, temp11, temp12, temp13;
	float temp20, temp21, temp22, temp23;
	float temp30, temp31, temp32, temp33;

	temp00 = left->m00 * right->m00
		+ left->m01 * right->m10
		+ left->m02 * right->m20
		+ left->m03 * right->m30;
	temp01 = left->m00 * right->m01
		+ left->m01 * right->m11
		+ left->m02 * right->m21
		+ left->m03 * right->m31;
	temp02 = left->m00 * right->m02
		+ left->m01 * right->m12
		+ left->m02 * right->m22
		+ left->m03 * right->m32;
	temp03 = left->m00 * right->m03
		+ left->m01 * right->m13
		+ left->m02 * right->m23
		+ left->m03 * right->m33;

	temp10 = left->m10 * right->m00
		+ left->m11 * right->m10
		+ left->m12 * right->m20
		+ left->m13 * right->m30;
	temp11 = left->m10 * right->m01
		+ left->m11 * right->m11
		+ left->m12 * right->m21
		+ left->m13 * right->m31;
	temp12 = left->m10 * right->m02
		+ left->m11 * right->m12
		+ left->m12 * right->m22
		+ left->m13 * right->m32;
	temp13 = left->m10 * right->m03
		+ left->m11 * right->m13
		+ left->m12 * right->m23
		+ left->m13 * right->m33;

	temp20 = left->m20 * right->m00
		+ left->m21 * right->m10
		+ left->m22 * right->m20
		+ left->m23 * right->m30;
	temp21 = left->m20 * right->m01
		+ left->m21 * right->m11
		+ left->m22 * right->m21
		+ left->m23 * right->m31;
	temp22 = left->m20 * right->m02
		+ left->m21 * right->m12
		+ left->m22 * right->m22
		+ left->m23 * right->m32;
	temp23 = left->m20 * right->m03
		+ left->m21 * right->m13
		+ left->m22 * right->m23
		+ left->m23 * right->m33;

	temp30 = left->m30 * right->m00
		+ left->m31 * right->m10
		+ left->m32 * right->m20
		+ left->m33 * right->m30;
	temp31 = left->m30 * right->m01
		+ left->m31 * right->m11
		+ left->m32 * right->m21
		+ left->m33 * right->m31;
	temp32 = left->m30 * right->m02
		+ left->m31 * right->m12
		+ left->m32 * right->m22
		+ left->m33 * right->m32;
	temp33 = left->m30 * right->m03
		+ left->m31 * right->m13
		+ left->m32 * right->m23
		+ left->m33 * right->m33;

	matrix.m00 = temp00;
	matrix.m01 = temp01;
	matrix.m02 = temp02;
	matrix.m03 = temp03;
	matrix.m10 = temp10;
	matrix.m11 = temp11;
	matrix.m12 = temp12;
	matrix.m13 = temp13;
	matrix.m20 = temp20;
	matrix.m21 = temp21;
	matrix.m22 = temp22;
	matrix.m23 = temp23;
	matrix.m30 = temp30;
	matrix.m31 = temp31;
	matrix.m32 = temp32;
	matrix.m33 = temp33;

	return matrix;
}

Matrix MathMatrixMultiplyScalar(Matrix* matrix,float scalar)
{
	matrix->m00 *= scalar;
	matrix->m01 *= scalar;
	matrix->m02 *= scalar;
	matrix->m03 *= scalar;
	matrix->m10 *= scalar;
	matrix->m11 *= scalar;
	matrix->m12 *= scalar;
	matrix->m13 *= scalar;
	matrix->m20 *= scalar;
	matrix->m21 *= scalar;
	matrix->m22 *= scalar;
	matrix->m23 *= scalar;
	matrix->m30 *= scalar;
	matrix->m31 *= scalar;
	matrix->m32 *= scalar;
	matrix->m33 *= scalar;
}

void MathMatrixMultiplyVector3(Matrix* matrix,float x,float y,float z,float *store)
{
	store[0] = matrix->m00 * x + matrix->m01 * y + matrix->m02 * z + matrix->m03;
	store[1] = matrix->m10 * x + matrix->m11 * y + matrix->m12 * z + matrix->m13;
	store[2] = matrix->m20 * x + matrix->m21 * y + matrix->m22 * z + matrix->m23;
}

Matrix MathMatrixTranspose(Matrix* matrix)
{
	Matrix nMatrix = MathMatrixCreate(matrix);
	float tmp = nMatrix.m01;
	nMatrix.m01 = nMatrix.m10;
	nMatrix.m10 = tmp;

	tmp = nMatrix.m02;
	nMatrix.m02 = nMatrix.m20;
	nMatrix.m20 = tmp;

	tmp = nMatrix.m03;
	nMatrix.m03 = nMatrix.m30;
	nMatrix.m30 = tmp;

	tmp = nMatrix.m12;
	nMatrix.m12 = nMatrix.m21;
	nMatrix.m21 = tmp;

	tmp = nMatrix.m13;
	nMatrix.m13 = nMatrix.m31;
	nMatrix.m31 = tmp;

	tmp = nMatrix.m23;
	nMatrix.m23 = nMatrix.m32;
	nMatrix.m32 = tmp;

	return nMatrix;
}

Matrix MathMatrixInvert(Matrix* matrix,BOOL* success)
{
	Matrix m2;
	float fInvDet;
	float fA0 = matrix->m00 * matrix->m11 - matrix->m01 * matrix->m10;
	float fA1 = matrix->m00 * matrix->m12 - matrix->m02 * matrix->m10;
	float fA2 = matrix->m00 * matrix->m13 - matrix->m03 * matrix->m10;
	float fA3 = matrix->m01 * matrix->m12 - matrix->m02 * matrix->m11;
	float fA4 = matrix->m01 * matrix->m13 - matrix->m03 * matrix->m11;
	float fA5 = matrix->m02 * matrix->m13 - matrix->m03 * matrix->m12;
	float fB0 = matrix->m20 * matrix->m31 - matrix->m21 * matrix->m30;
	float fB1 = matrix->m20 * matrix->m32 - matrix->m22 * matrix->m30;
	float fB2 = matrix->m20 * matrix->m33 - matrix->m23 * matrix->m30;
	float fB3 = matrix->m21 * matrix->m32 - matrix->m22 * matrix->m31;
	float fB4 = matrix->m21 * matrix->m33 - matrix->m23 * matrix->m31;
	float fB5 = matrix->m22 * matrix->m33 - matrix->m23 * matrix->m32;
	float fDet = fA0 * fB5 - fA1 * fB4 + fA2 * fB3 + fA3 * fB2 - fA4 * fB1 + fA5 * fB0;

	if (MathFloatEqual(fDet,0.0f)) {
		if(success!=NULL)
			*success = FALSE;
		return *matrix;
	}

	m2.m00 = +matrix->m11 * fB5 - matrix->m12 * fB4 + matrix->m13 * fB3;
	m2.m10 = -matrix->m10 * fB5 + matrix->m12 * fB2 - matrix->m13 * fB1;
	m2.m20 = +matrix->m10 * fB4 - matrix->m11 * fB2 + matrix->m13 * fB0;
	m2.m30 = -matrix->m10 * fB3 + matrix->m11 * fB1 - matrix->m12 * fB0;
	m2.m01 = -matrix->m01 * fB5 + matrix->m02 * fB4 - matrix->m03 * fB3;
	m2.m11 = +matrix->m00 * fB5 - matrix->m02 * fB2 + matrix->m03 * fB1;
	m2.m21 = -matrix->m00 * fB4 + matrix->m01 * fB2 - matrix->m03 * fB0;
	m2.m31 = +matrix->m00 * fB3 - matrix->m01 * fB1 + matrix->m02 * fB0;
	m2.m02 = +matrix->m31 * fA5 - matrix->m32 * fA4 + matrix->m33 * fA3;
	m2.m12 = -matrix->m30 * fA5 + matrix->m32 * fA2 - matrix->m33 * fA1;
	m2.m22 = +matrix->m30 * fA4 - matrix->m31 * fA2 + matrix->m33 * fA0;
	m2.m32 = -matrix->m30 * fA3 + matrix->m31 * fA1 - matrix->m32 * fA0;
	m2.m03 = -matrix->m21 * fA5 + matrix->m22 * fA4 - matrix->m23 * fA3;
	m2.m13 = +matrix->m20 * fA5 - matrix->m22 * fA2 + matrix->m23 * fA1;
	m2.m23 = -matrix->m20 * fA4 + matrix->m21 * fA2 - matrix->m23 * fA0;
	m2.m33 = +matrix->m20 * fA3 - matrix->m21 * fA1 + matrix->m22 * fA0;

	fInvDet = 1.0f / fDet;
	m2 = MathMatrixMultiplyScalar(&m2,fInvDet);

	if(success!=NULL)
		*success = TRUE;
	return m2;
}

void MathMatrixSetValue(Matrix* matrix,int y,int x,float v)
{
	float *f;
	if(y<0||y>3||x<0||x>3)
		return;
	f = &matrix->m00;
	f += y+(x*4);
	*f = v;
}

Quaternion MathQuaternionCreate(Quaternion* source)
{
	Quaternion quaternion;
	if(source!=NULL)
	{
		quaternion = *source;
	}
	else
	{
		MathQuaternionLoadIdentity(&quaternion);
	}
	return quaternion;
}

void MathQuaternionLoadIdentity(Quaternion* quaternion)
{
	quaternion->w = 1.0f;
	quaternion->x = 0.0f;
	quaternion->y = 0.0f;
	quaternion->z = 0.0f;
}

Matrix MathQuaternionToMatrix(Quaternion* source)
{
	Matrix result = MathMatrixCreate(NULL);
	float x = source->x;
	float y = source->y;
	float z = source->z;
	float w = source->w;
	float norm = w * w + x * x + y * y + z * z;


	float s = (norm == 1.0f) ? 2.0f : (norm > 0.0f) ? 2.0f / norm : 0;

	float xs = x * s;
	float ys = y * s;
	float zs = z * s;
	float xx = x * xs;
	float xy = x * ys;
	float xz = x * zs;
	float xw = w * xs;
	float yy = y * ys;
	float yz = y * zs;
	float yw = w * ys;
	float zz = z * zs;
	float zw = w * zs;

	result.m00 = 1 - (yy + zz);
	result.m01 = (xy - zw);
	result.m02 = (xz + yw);
	result.m10 = (xy + zw);
	result.m11 = 1 - (xx + zz);
	result.m12 = (yz - xw);
	result.m20 = (xz - yw);
	result.m21 = (yz + xw);
	result.m22 = 1 - (xx + yy);

	return result;
	/*Matrix matrix;

	matrix.m00=matrix.m11=matrix.m22=matrix.m33 = source->w;

	matrix.m10=matrix.m32 = source->x;
	matrix.m01=matrix.m23 = -source->x;

	matrix.m30=matrix.m21 = source->y;
	matrix.m12=matrix.m03 = -source->y;

	matrix.m02=matrix.m31 = source->z;
	matrix.m20=matrix.m13 = -source->z;

	return matrix;*/
}

Quaternion MathQuaternionSlerp(Quaternion* q1,Quaternion* q2,float rate)
{
	float result;
	BOOL shouldNegate = FALSE;
	float scale0;
	float scale1;
	Quaternion quaternion;
	if(MathFloatEqual(q1->x,q2->x) && 
		MathFloatEqual(q1->y,q2->y)&&
		MathFloatEqual(q1->z,q2->z))
		return *q1;
	result = (q1->x * q2->x) + (q1->y * q2->y) + (q1->z * q2->z) + (q1->w * q2->w);
	if(result<0.0f)
	{
		shouldNegate = TRUE;
		result = -result;
	}

	scale0 = 1 - rate;
	scale1 = rate;
	if((1 - result) > 0.1f)
	{
		float theta = acosf(result);
		float invSinTheta = 1.0f / sinf(theta);
		scale0 = sin((1 - rate) * theta) * invSinTheta;
		scale1 = sin((rate * theta)) * invSinTheta;
	}
	if(!shouldNegate)
	{
		quaternion.x = (scale0 * q1->x) + (scale1 * q2->x);
		quaternion.y = (scale0 * q1->y) + (scale1 * q2->y);
		quaternion.z = (scale0 * q1->z) + (scale1 * q2->z);
		quaternion.w = (scale0 * q1->w) + (scale1 * q2->w);
	}
	else
	{
		quaternion.x = (scale0 * q1->x) + (scale1 * -q2->x);
		quaternion.y = (scale0 * q1->y) + (scale1 * -q2->y);
		quaternion.z = (scale0 * q1->z) + (scale1 * -q2->z);
		quaternion.w = (scale0 * q1->w) + (scale1 * -q2->w);
	}
	return quaternion;
}

Quaternion MathQuaternionMultiplyQuaternion( Quaternion* q1,Quaternion* q2 )
{
	Quaternion quaternion;
	float qw = q2->w, qx = q2->x, qy = q2->y, qz = q2->z;
	quaternion.x = q1->x * qw + q1->y * qz - q1->z * qy + q1->w * qx;
	quaternion.y = -q1->x * qz + q1->y * qw + q1->z * qx + q1->w * qy;
	quaternion.z = q1->x * qy - q1->y * qx + q1->z * qw + q1->w * qz;
	quaternion.w = -q1->x * qx - q1->y * qy - q1->z * qz + q1->w * qw;
	return quaternion;
}

void MathQuaternionMultiplyVector3( Quaternion* q,float vx,float vy,float vz,float *store )
{
	if(vx==0 && vy==0 && vz==0)
	{
		store[0]=0;
		store[1]=0;
		store[2]=0;
	}
	else
	{
		float x = q->x, y = q->y, z = q->z, w = q->w;
		store[0] = w * w * vx + 2 * y * w * vz - 2 * z * w * vy + x * x
			* vx + 2 * y * x * vy + 2 * z * x * vz - z * z * vx - y
			* y * vx;
		store[1] = 2 * x * y * vx + y * y * vy + 2 * z * y * vz + 2 * w
			* z * vx - z * z * vy + w * w * vy - 2 * x * w * vz - x
			* x * vy;
		store[2] = 2 * x * z * vx + 2 * y * z * vy + z * z * vz - 2 * w
			* y * vx - y * y * vz + 2 * w * x * vy - x * x * vz + w
			* w * vz;
	}
}
