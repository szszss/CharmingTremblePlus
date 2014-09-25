#pragma once
#include "game.h"

struct implQuaternion
{
	float w,x,y,z;
};

struct implMatrix
{
	float m00,m10,m20,m30;
	float m01,m11,m21,m31;
	float m02,m12,m22,m32;
	float m03,m13,m23,m33;
	/*
	不要被外表迷惑了,其实如果写成数学中正规的矩阵的话,应该是:
	m00 m01 m02 m03
	m10 m11 m12 m13
	m20 m21 m22 m23
	m30 m31 m32 m33
	*/
};

void MathInit();
BOOL MathFloatEqual(float a,float b);
BOOL MathDoubleEqual(double a,double b);
unsigned long MathNextMultiple8(unsigned long v);
unsigned long MathNextPower2(unsigned long v);

Matrix MathMatrixCreate(Matrix* source);
void MathMatrixLoadIdentity(Matrix* matrix);
Matrix MathMatrixMultiplyMatrix(Matrix* left,Matrix* right);
Matrix MathMatrixMultiplyScalar(Matrix* matrix,float scalar);
void MathMatrixMultiplyVector3(Matrix* matrix,float x,float y,float z,float *store);
Matrix MathMatrixTranspose(Matrix* matrix);
Matrix MathMatrixInvert(Matrix* matrix,BOOL* success);
void MathMatrixSetValue(Matrix* matrix,int y,int x,float v);

Quaternion MathQuaternionCreate(Quaternion* source);
void MathQuaternionLoadIdentity(Quaternion* quaternion);
Matrix MathQuaternionToMatrix(Quaternion* source);
Quaternion MathQuaternionSlerp(Quaternion* q1,Quaternion* q2,float rate);
Quaternion MathQuaternionMultiplyQuaternion(Quaternion* q1,Quaternion* q2);
void MathQuaternionMultiplyVector3(Quaternion* q,float vx,float vy,float vz,float *store);

