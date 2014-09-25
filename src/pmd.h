#ifndef pmd_h__
#define pmd_h__

#include "game.h"
#include "math.h"

struct implPMD_Vertex
{
	float x;
	float y;
	float z;
	float nx;
	float nz;
	float ny;
	float u;
	float v;
	unsigned short bone0;
	unsigned short bone1;
	float weight;
	char edge;
};

struct implPMD_Model
{
	char *name;
	PMD_Vertex *vertexs;
	unsigned short *indexes;
	PMD_Material *materials;
	PMD_Bone *bones;
	//HashTree *boneMapping;
	unsigned long vertexCount;
	unsigned long indexCount;
	unsigned long materialCount;
	unsigned short boneCount;
};

struct implPMD_Bone
{
	char name[21];
	char boneType;
	short parent;
	short child;
	short targetBone;
	float posX;
	float posY;
	float posZ;
};

struct implPMD_BoneInstance
{
	PMD_Bone *bone;
	int pass;
	float nowPosX;
	float nowPosY;
	float nowPosZ;
	Quaternion localRot;
	Quaternion worldRot;
	Matrix localTransformMatrix; //局部变形矩阵(相对变形矩阵)
	Matrix worldTransformMatrix; //世界变形矩阵(绝对变形矩阵)
	PMD_KeyFrame *firstKeyFrame; //第一个关键帧
	PMD_KeyFrame *currentKeyFrame; //当前指向的关键帧
	/*
	pass表示当前骨骼的变换阶段和状态.
	低4位 - 阶段:
	0000 0 尚未进行任何变换
	0001 1 相对变换阶段
	0010 2 绝对变换阶段
	低5~8位 - 状态:
	0000 0 未曾设置 (无需顶点变换)
	0001 1 设置了相对变换矩阵
	0010 2 未设置相对变换矩阵,但父骨骼中存在被设置了相对变换矩阵的.
	0011 2 设置了相对变换矩阵,且父骨骼中存在被设置了相对变换矩阵的.
	*/
};

struct implPMD_BoneHierarchy
{
	PMD_BoneInstance *boneInstaces;
};

struct implPMD_Animation
{
	char *name;
	long frameLength; //总帧长
	HashTree *frameHT; //以骨骼名为Key,存储firstKeyFrame的HashTree
};

struct implPMD_KeyFrame
{
	long currentFrameIndex;//当前关键帧的帧序号
	PMD_KeyFrame *nextFrame;//下一个关键帧,如果没有的话,则为NULL
	char *boneName;//对应的骨骼的名称
	float posX;
	float posY;
	float posZ;
	Quaternion rot;
};

struct implPMD_AnimationPlayer
{
	PMD_Animation *animation; //当前的动作
	long currentFrameIndex; //当前的帧序号
};

struct implPMD_ModelInstance
{
	PMD_Model *model;
	PMD_BoneHierarchy boneHierarchy;
	PMD_AnimationPlayer animationPlayer;
	/*double posX;
	double posY;
	double posZ;
	double homogeneity;
	double scaleX;
	double scaleY;
	double scaleZ;*/
};

struct implPMD_Material
{
	float diffuseR;
	float diffuseG;
	float diffuseB;
	float diffuseA;
	float shininess;
	float specularR;
	float specularG;
	float specularB;
	float ambientR;
	float ambientG;
	float ambientB;
	byte toonNumber;
	byte edgeFlag;
	unsigned long faceAmount;
	Texture *texture;
};

void PMD_Init();
void PMD_Close();

/*装载一个模型,basePath为基础路径,fileName为模型文件名*/
PMD_Model* PMD_LoadModel(char *basePath,char *fileName);
/*为一个已加载的模型创建一个实例*/
PMD_ModelInstance* PMD_ModelInstanceCreate(PMD_Model *model);
/*销毁模型实例*/
void PMD_ModelInstanceDestroy(PMD_ModelInstance *modelInstance);
/*渲染模型实例*/
void PMD_ModelInstanceRender(PMD_ModelInstance *modelInstance);
//PMD_Model* PMD_DestroyModel(PMD_Model *model);
PMD_Animation* PMD_LoadAnimation(char *basePath,char *fileName);
void PMD_UseAnimation(PMD_ModelInstance *modelInstance,PMD_Animation *animation);
void PMD_AnimationTick(PMD_ModelInstance *modelInstance);

#endif // pmd_h__
