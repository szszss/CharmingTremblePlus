#include "pmd.h"
#include "memory.h"
#include "collection.h"
#include "util.h"
#include "resourcemanager.h"
#include "renderengine.h"
#include "SDL_opengl.h"
#include <memory.h>

HashTree *modelMap;
HashTree *animationMap;
PMD_Animation *animationRun = NULL;
PMD_Animation *animationStand = NULL;

BOOL PMD_CheckMagicNumber(FILE *file);
BOOL PMD_ReadHeader(PMD_Model *model,FILE *file);
BOOL PMD_ReadVertext(PMD_Model *model,FILE *file);
BOOL PMD_ReadIndex(PMD_Model *model,FILE *file);
BOOL PMD_ReadMaterial(PMD_Model *model,FILE *file,char *basePath);
BOOL PMD_ReadBone(PMD_Model *model,FILE *file);
BOOL PMD_MixMatrix(Matrix *result,Matrix *m1,Matrix *m2,float weight);
BOOL VMD_CheckMagicNumber(FILE *file);
//BOOL VMD_ReadHeader(PMD_Animation *animation,FILE *file);
BOOL VMD_ReadBoneKeyFrame(PMD_Animation *animation,FILE *file);
void CallbackModelDestroy(void *_model);
void CallbackAnimationDestroy(void *_animation);
void CallbackBoneDestroy(void *_bone);

void PMD_Init()
{
	modelMap = HashTreeCreate();
	animationMap = HashTreeCreate();
	animationRun = PMD_LoadAnimation("animation","run.vmd");
	animationStand = PMD_LoadAnimation("animation","stand.vmd");
	LoggerInfo("PMD initialized");
}

void PMD_Close()
{
	HashTreeDestroy(modelMap,CallbackModelDestroy);
	HashTreeDestroy(animationMap,CallbackAnimationDestroy);
	LoggerInfo("PMD closed");
}

void CallbackModelDestroy(void *_model)
{
	PMD_Model *model = (PMD_Model*)_model;
	free_s(model->name);
	free_s(model->indexes);
	free_s(model->materials);
	free_s(model->vertexs);
	//HashTreeDestroy(model->boneMapping,CallbackBoneDestroy);
	/*for(i=0;i<model->boneCount;i++)
	{
		free_s(model->bones[i].name);
	}*/
	free_s(model->bones);
	free_s(model);
}

void CallbackKeyFrameDestroy(void *_firstKeyFrame)
{
	PMD_KeyFrame *firstKeyFrame = (PMD_KeyFrame*)_firstKeyFrame;
	while(firstKeyFrame!=NULL)
	{
		PMD_KeyFrame *old = firstKeyFrame;
		firstKeyFrame = firstKeyFrame->nextFrame;
		free_s(old->boneName);
		free_s(old);
	}
}

void CallbackAnimationDestroy(void *_animation)
{
	int i;
	PMD_Animation *animation = (PMD_Animation*)_animation;
	HashTreeDestroy(animation->frameHT,CallbackKeyFrameDestroy);
	free_s(animation->name);
	free_s(animation);
}

void CallbackBoneDestroy(void *_bone)
{
	unsigned short *id = (unsigned short*)_bone;
	free_s(id);
}

PMD_Model* PMD_LoadModel(char *basePath,char *fileName)
{
	FILE *file;
	PMD_Model *model = NULL;
	StringBuilder *sb = NULL;
	char *filePath = NULL;
	char *newBasePath = NULL;
	BOOL result;
	if(basePath==NULL||fileName==NULL)
	{
		return NULL;
	}
	sb = SBCreate();
	SBAppend(sb,basePath);
	SBAppend(sb,"/");
	newBasePath = SBBuildWithoutDestroy(sb);
	SBAppend(sb,fileName);
	filePath = SBBuild(sb);
	model = (PMD_Model*)HashTreeGet(modelMap,filePath,&result);
	if(result)
	{
		free_s(filePath);
		free_s(newBasePath);
		return model;
	}
	file = fopen(filePath,"rb");
	if(file==NULL)
	{
		free_s(filePath);
		free_s(newBasePath);
		return NULL;
	}
	if(!PMD_CheckMagicNumber(file))
	{
		free_s(filePath);
		free_s(newBasePath);
		return NULL;
	}
	model = (PMD_Model*)malloc_s(sizeof(PMD_Model));
	model->name = filePath;
	PMD_ReadHeader(model,file);
	PMD_ReadVertext(model,file);
	PMD_ReadIndex(model,file);
	PMD_ReadMaterial(model,file,newBasePath);
	PMD_ReadBone(model,file);
	free_s(newBasePath);
	fclose(file);
	HashTreeAdd(modelMap,filePath,model);
	return model;
}

BOOL PMD_CheckMagicNumber(FILE *file)
{
	char magicNumber[3];
	magicNumber[0] = LESReadChar(file);
	magicNumber[1] = LESReadChar(file);
	magicNumber[2] = LESReadChar(file);
	if(magicNumber[0]=='P' && magicNumber[1]=='m' && magicNumber[2]=='d')
	{
		return TRUE;
	}
	return FALSE;
}

BOOL PMD_ReadHeader(PMD_Model *model,FILE *file)
{
	float modelVersion;
	byte *rubbish = (byte*)malloc_s(256*sizeof(byte));
	modelVersion = LESReadFloat(file);
	LESReadBytes(file,rubbish,20);
	LESReadBytes(file,rubbish,256);
	free_s(rubbish);
	return !feof(file);
}

BOOL PMD_ReadVertext(PMD_Model *model,FILE *file)
{
	int i;
	unsigned long count = LESReadUInt32(file);
	model->vertexCount = count;
	model->vertexs = (PMD_Vertex *)malloc_s(count*sizeof(PMD_Vertex));
	for(i=0;i<count;i++)
	{
		model->vertexs[i].x=LESReadFloat(file);
		model->vertexs[i].y=LESReadFloat(file);
		model->vertexs[i].z=LESReadFloat(file);
		model->vertexs[i].nx=LESReadFloat(file);
		model->vertexs[i].ny=LESReadFloat(file);
		model->vertexs[i].nz=LESReadFloat(file);
		model->vertexs[i].u=LESReadFloat(file);
		model->vertexs[i].v=LESReadFloat(file);
		model->vertexs[i].bone0=LESReadUInt16(file);
		model->vertexs[i].bone1=LESReadUInt16(file);
		model->vertexs[i].weight=LESReadByte(file)/100.0f;	
		model->vertexs[i].edge=LESReadChar(file);
		if(feof(file))
			return FALSE;
	}
	return !feof(file);
}

BOOL PMD_ReadIndex(PMD_Model *model,FILE *file)
{
	int i;
	unsigned long count = LESReadUInt32(file);
	model->indexCount = count;
	model->indexes = (unsigned short *)malloc_s(count*sizeof(unsigned short));
	for(i=0;i<count;i++)
	{
		model->indexes[i] = LESReadUInt16(file);
	}
	return !feof(file);
}

BOOL PMD_ReadMaterial(PMD_Model *model,FILE *file,char *basePath)
{
	int i;
	unsigned long count = LESReadUInt32(file);
	StringBuilder *sb = NULL;
	char *buffer = (char *)malloc_s(21*sizeof(char)); //防溢出
	char *textureName = NULL;
	model->materialCount = count;
	model->materials = (PMD_Material *)malloc_s(count*sizeof(PMD_Material));
	for(i=0;i<count;i++)
	{
		model->materials[i].diffuseR=LESReadFloat(file);
		model->materials[i].diffuseG=LESReadFloat(file);
		model->materials[i].diffuseB=LESReadFloat(file);
		model->materials[i].diffuseA=LESReadFloat(file);
		model->materials[i].shininess=LESReadFloat(file);
		model->materials[i].specularR=LESReadFloat(file);
		model->materials[i].specularG=LESReadFloat(file);
		model->materials[i].specularB=LESReadFloat(file);
		model->materials[i].ambientR=LESReadFloat(file);
		model->materials[i].ambientG=LESReadFloat(file);
		model->materials[i].ambientB=LESReadFloat(file);
		model->materials[i].toonNumber=LESReadByte(file);
		model->materials[i].edgeFlag=LESReadByte(file);
		model->materials[i].faceAmount=LESReadUInt32(file)/3;
		memset(buffer, 0, 21*sizeof(char));
		LESReadBytes(file,(byte*)buffer,20);
		sb = SBCreate();
		SBAppend(sb,basePath);
		SBAppend(sb,buffer);
		textureName = SBBuild(sb);
		model->materials[i].texture=RM_GetTexture(textureName);
		free_s(textureName);
	}
	free_s(buffer);
	return !feof(file);
}

BOOL PMD_ReadBone(PMD_Model *model,FILE *file)
{
	unsigned short i;
	unsigned short count = LESReadUInt16(file);
	model->boneCount = count;
	model->bones = (PMD_Bone *)malloc_s(count*sizeof(PMD_Bone));
	//model->boneMapping = HashTreeCreate(); //不要忘了销毁时释放字符串内存(已实现)
	for(i=0;i<count;i++)
	{
		unsigned short *number = (unsigned short*)malloc_s(sizeof(unsigned short));
		//char *buffer = (char *)malloc_s(21*sizeof(char)); //防溢出
		memset(model->bones[i].name, 0, 21*sizeof(char));
		LESReadBytes(file,(byte*)model->bones[i].name,20);
		*number = i;
		//HashTreeAdd(model->boneMapping, (model->bones[i].name) ,number);
		model->bones[i].parent = LESReadInt16(file);
		model->bones[i].child = LESReadInt16(file);
		model->bones[i].boneType = LESReadByte(file);
		model->bones[i].targetBone = LESReadInt16(file);
		model->bones[i].posX = LESReadFloat(file);
		model->bones[i].posY = LESReadFloat(file);
		model->bones[i].posZ = LESReadFloat(file);
	}
}

PMD_ModelInstance* PMD_ModelInstanceCreate(PMD_Model *model)
{
	int i;
	PMD_ModelInstance *modelInstance;
	PMD_BoneInstance *boneInstance;
	if(model==NULL)
		return NULL;
	modelInstance = (PMD_ModelInstance*)malloc_s(sizeof(PMD_ModelInstance));
	modelInstance->model = model;
	modelInstance->boneHierarchy.boneInstaces = (PMD_BoneInstance *)malloc_s(model->boneCount*sizeof(PMD_BoneInstance));
	for(i=0;i<model->boneCount;i++)
	{
		boneInstance = modelInstance->boneHierarchy.boneInstaces+i;
		boneInstance->bone = &model->bones[i];
		boneInstance->pass = 0;
		boneInstance->localTransformMatrix = MathMatrixCreate(NULL);
		boneInstance->worldTransformMatrix = MathMatrixCreate(NULL);
	}
	modelInstance->animationPlayer.currentFrameIndex=0;
	modelInstance->animationPlayer.animation=NULL;
	return modelInstance;
}

void PMD_ModelInstanceDestroy(PMD_ModelInstance *modelInstance)
{
	if(modelInstance==NULL)
		return;
	free_s(modelInstance->boneHierarchy.boneInstaces);
	free_s(modelInstance);
}

/*
void CallbackBoneMappingDestroy(void* _boneMapping)
{
	unsigned short *boneNumber = (unsigned short *)_boneMapping;
	free_s(boneNumber);
}

PMD_Model* PMD_DestroyModel(PMD_Model *model)
{
	free_s(model->indexes);
	free_s(model->materials);
	free_s(model->name);
	free_s(model->vertexs);
	HashTreeDestroy(model->boneMapping,&CallbackBoneMappingDestroy);
	free_s(model->bones); //骨骼内存释放得在映射表之后进行,这一点很重要.
	free_s(model);
}*/

BOOL PMD_MixMatrix(Matrix *result,PMD_BoneInstance *b0,PMD_BoneInstance *b1,float weight)
{
	if(weight>0.99f)
	{
		if((b0->pass&48) == 0)
			return FALSE;
		*result = b0->worldTransformMatrix;
		//result->m00 = 2.0f;
	}
	else if(weight>0.01f)
	{
		int i;
		if(((b0->pass&48) == 0) && ((b1->pass&48) == 0))
			return FALSE;
		for(i=0;i<16;i++)
		{
			*(&(result->m00)+i) = *(&(b0->worldTransformMatrix.m00)+i) * weight + *(&(b1->worldTransformMatrix.m00)+i) * (1.0f - weight);
		}
		//result->m00 = 2.0f;
	}
	else
	{
		if((b1->pass&48) == 0)
			return FALSE;
		*result = b1->worldTransformMatrix;
	}
	return TRUE;
}

void PMD_ModelInstanceRender(PMD_ModelInstance *modelInstance)
{
	static float scale = 0.15f;
	//static float scale = 0.55f;
	float vbuffer[3];
	Matrix matrix;
	Matrix *m1;
	Matrix *m2;
	int i,j,face;
	long materialPointer=-1;
	long materialThreshold=2000000000;
	long vertex;
	PMD_Model *model = modelInstance->model;
	int indexCount = model->indexCount;
	//PMD_AnimationTick(modelInstance);
	if(model->materialCount>0)
	{
		materialThreshold=0;		
	}
	glPushAttrib(GL_ALL_ATTRIB_BITS);
	//glEnable(GL_LIGHTING);
	//glShadeModel(GL_SMOOTH);
	glDisable(GL_CULL_FACE);
	glScalef(scale,scale,-scale);
	//glRotatef(-90,0,1,0);
	glColor3f(1,1,1);
	for(i=0,face=0;i<indexCount;i++,face++)
	{
		if(face==materialThreshold)
		{
			materialPointer++;
			materialThreshold+=model->materials[materialPointer].faceAmount;
			RE_BindTexture(model->materials[materialPointer].texture);
			RE_SetMaterial(&(model->materials[materialPointer].diffuseR),
				&(model->materials[materialPointer].ambientR),
				&(model->materials[materialPointer].specularR),
				&(model->materials[materialPointer].shininess));
		}
		glBegin(GL_TRIANGLES);
		for(j=2;j>=0;j--)
		{
			vertex = model->indexes[i+j];
			vbuffer[0]=model->vertexs[vertex].x;
			vbuffer[1]=model->vertexs[vertex].y;
			vbuffer[2]=model->vertexs[vertex].z;
			if(!PMD_MixMatrix(&matrix, 
				&modelInstance->boneHierarchy.boneInstaces[model->vertexs[vertex].bone0],
				&modelInstance->boneHierarchy.boneInstaces[model->vertexs[vertex].bone1],
				model->vertexs[vertex].weight))
			{
				vbuffer[0]=model->vertexs[vertex].x;
				vbuffer[1]=model->vertexs[vertex].y;
				vbuffer[2]=model->vertexs[vertex].z;
			}
			else
			{
				MathMatrixMultiplyVector3(&matrix,model->vertexs[vertex].x, model->vertexs[vertex].y, model->vertexs[vertex].z,vbuffer);
			}
			glNormal3f(model->vertexs[vertex].nx, model->vertexs[vertex].ny, model->vertexs[vertex].nz);
			glTexCoord2f(model->vertexs[vertex].u,model->vertexs[vertex].v);
			glVertex3f(vbuffer[0], vbuffer[1], vbuffer[2]);
		}
		i+=2;
		glEnd();
	}
	RE_BindTexture(NULL);
	glPopAttrib();
}

PMD_Animation* PMD_LoadAnimation(char *basePath,char *fileName)
{
	FILE *file;
	PMD_Animation *animation = NULL;
	StringBuilder *sb = NULL;
	char *filePath = NULL;
	char *newBasePath = NULL;
	BOOL result;
	if(basePath==NULL||fileName==NULL)
	{
		return NULL;
	}
	sb = SBCreate();
	SBAppend(sb,basePath);
	SBAppend(sb,"/");
	newBasePath = SBBuildWithoutDestroy(sb);
	SBAppend(sb,fileName);
	filePath = SBBuild(sb);
	animation = (PMD_Animation*)HashTreeGet(animationMap,filePath,&result);
	if(result)
	{
		free_s(filePath);
		free_s(newBasePath);
		return animation;
	}
	file = fopen(filePath,"rb");
	if(file==NULL)
	{
		free_s(filePath);
		free_s(newBasePath);
		return NULL;
	}
	if(!VMD_CheckMagicNumber(file))
	{
		free_s(filePath);
		free_s(newBasePath);
		return NULL;
	}
	animation = (PMD_Animation*)malloc_s(sizeof(PMD_Animation));
	animation->name = filePath;
	//VMD_ReadHeader(animation,file);
	VMD_ReadBoneKeyFrame(animation,file);
	free_s(newBasePath);
	fclose(file);
	HashTreeAdd(animationMap,filePath,animation);
	return animation;
}

BOOL VMD_CheckMagicNumber(FILE *file)
{
	char magicNumber[30];
	memset(magicNumber,0,30*sizeof(char));
	LESReadBytes(file,(byte*)magicNumber,30);
	if(strcmp(magicNumber,"Vocaloid Motion Data file")==0)
	{
		LESReadBytes(file,(byte*)magicNumber,10);
		return TRUE;
	}
	else if(strcmp(magicNumber,"Vocaloid Motion Data 0002")==0)
	{
		LESReadBytes(file,(byte*)magicNumber,20);
		return TRUE;
	}
	return FALSE;
}

typedef struct
{
	char *name;
	long currentFrame;
	float posX;
	float posY;
	float posZ;
	Quaternion rotQuaternion;
} _TEMP_KeyFrame;



HashTree *VMD_MakeKeyFrame(LinkedList *list,long maxFrame)
{
	int j = 0;
	LinkedListIterator *iterator;
	HashTree *hashTree = HashTreeCreate();
	for(iterator=LinkedListGetIterator(list);LinkedListIteratorHasNext(iterator);)
	{
		_TEMP_KeyFrame *tempFrame2 = (_TEMP_KeyFrame *)LinkedListIteratorGetNext(iterator);
		PMD_KeyFrame *keyFrame = (PMD_KeyFrame *)malloc_s(sizeof(PMD_KeyFrame));
		PMD_KeyFrame *firstKeyFrame = NULL;
		keyFrame->nextFrame=NULL;
		keyFrame->boneName=tempFrame2->name;
		keyFrame->currentFrameIndex=tempFrame2->currentFrame;
		keyFrame->posX=tempFrame2->posX;
		keyFrame->posY=tempFrame2->posY;
		keyFrame->posZ=tempFrame2->posZ;
		keyFrame->rot=tempFrame2->rotQuaternion;
		LinkedListIteratorDeleteCurrent(iterator);
		free_s(tempFrame2);
		j++;

		firstKeyFrame = (PMD_KeyFrame*)HashTreeGet(hashTree,keyFrame->boneName,NULL);
		if(firstKeyFrame==NULL)
		{
			HashTreeAdd(hashTree,keyFrame->boneName,keyFrame);
		}
		else
		{
			while(firstKeyFrame->nextFrame!=NULL)
			{
				firstKeyFrame=firstKeyFrame->nextFrame;
			}
			firstKeyFrame->nextFrame=keyFrame;
		}
	}
	LinkedListDestory(list,NULL);
	return hashTree;
}

BOOL VMD_ReadBoneKeyFrame(PMD_Animation *animation,FILE *file)
{
	unsigned long i;
	unsigned long count = LESReadUInt32(file);
	unsigned long maxFrame = 0;
	byte dummyBytes[64];
	_TEMP_KeyFrame *tempFrame;
	LinkedList *list = LinkedListCreate();
	animation->name = (char *)malloc_s(1*sizeof(char));
	animation->name[0] = '\0';
	for(i=0;i<count;i++)
	{
		char *name = (char*)malloc_s(15*sizeof(char));
		long nowFrame;
		LESReadBytes(file,(byte*)name,15);
		nowFrame = LESReadUInt32(file);
		maxFrame = nowFrame>maxFrame?nowFrame:maxFrame;
		tempFrame = (_TEMP_KeyFrame *)malloc_s(sizeof(_TEMP_KeyFrame));
		tempFrame->name = name;
		tempFrame->currentFrame = nowFrame;
		tempFrame->posX=LESReadFloat(file);
		tempFrame->posY=LESReadFloat(file);
		tempFrame->posZ=LESReadFloat(file);
		tempFrame->rotQuaternion=MathQuaternionCreate(NULL);
		tempFrame->rotQuaternion.x=LESReadFloat(file);
		tempFrame->rotQuaternion.y=LESReadFloat(file);
		tempFrame->rotQuaternion.z=LESReadFloat(file);
		tempFrame->rotQuaternion.w=LESReadFloat(file);
		LESReadBytes(file,dummyBytes,64); //Skip interpolation data.
		/*if(nowFrame>postFrame)
		{
			VMD_MakeKeyFrame(list,nowFrame);
			postFrame=nowFrame;
		}*/
		LinkedListAdd(list,tempFrame);
	}
	animation->frameHT = VMD_MakeKeyFrame(list,maxFrame);
	animation->frameLength = maxFrame;
	return TRUE;
}

void PMD_UseAnimation(PMD_ModelInstance *modelInstance,PMD_Animation *animation)
{
	PMD_AnimationPlayer *player = &modelInstance->animationPlayer;
	int i;
	player->animation = animation;
	//player->currentKeyFrame = animation->keyFrame[0];
	player->currentFrameIndex = -1;
	for(i=0;i<(modelInstance->model->boneCount);i++)
	{
		PMD_BoneInstance *boneInstance = modelInstance->boneHierarchy.boneInstaces + i;
		BOOL result;
		PMD_KeyFrame *keyFrame = (PMD_KeyFrame*)HashTreeGet(player->animation->frameHT,boneInstance->bone->name,&result);
		if(result!=FALSE)
		{
			boneInstance->nowPosX = keyFrame->posX;
			boneInstance->nowPosY = keyFrame->posY;
			boneInstance->nowPosZ = keyFrame->posZ;
			boneInstance->pass=0;
			boneInstance->currentKeyFrame=keyFrame;
			boneInstance->firstKeyFrame=keyFrame;
			MathMatrixLoadIdentity(&(boneInstance->localTransformMatrix));
			boneInstance->localTransformMatrix.m03 = keyFrame->posX;
			boneInstance->localTransformMatrix.m13 = keyFrame->posY;
			boneInstance->localTransformMatrix.m23 = keyFrame->posZ;
			boneInstance->localRot = keyFrame->rot;
		}
		else
		{
			boneInstance->nowPosX = 0.0f;
			boneInstance->nowPosY = 0.0f;
			boneInstance->nowPosZ = 0.0f;
			boneInstance->pass=0;
			boneInstance->currentKeyFrame=NULL;
			boneInstance->firstKeyFrame=NULL;
			MathMatrixLoadIdentity(&(boneInstance->localTransformMatrix));
			MathQuaternionLoadIdentity(&(boneInstance->localRot));
		}	
	}
	//PMD_AnimationTick(modelInstance);
}

void PMD_AnimationBoneCala(PMD_ModelInstance *modelInstance,PMD_BoneInstance *boneInstance)
{
	Matrix tempMatrix;
	Matrix rotMatrix;
	if(boneInstance->bone->parent!=-1)
	{
		PMD_BoneInstance *parentInstance = modelInstance->boneHierarchy.boneInstaces + boneInstance->bone->parent;
		float result[3];
		if(parentInstance->pass!=48)
			PMD_AnimationBoneCala(modelInstance,parentInstance);
		/*boneInstance->localTransformMatrix.m03 = boneInstance->bone->posX - parentInstance->bone->posX + parentInstance->localTransformMatrix.m03;
		boneInstance->localTransformMatrix.m13 = boneInstance->bone->posY - parentInstance->bone->posY + parentInstance->localTransformMatrix.m13;
		boneInstance->localTransformMatrix.m23 = boneInstance->bone->posZ - parentInstance->bone->posZ + parentInstance->localTransformMatrix.m23;
		boneInstance->worldRot = MathQuaternionMultiplyQuaternion(&parentInstance->worldRot,&boneInstance->localRot);
		//MathMatrixMultiplyVector3(&rotMatrix,boneInstance->localTransformMatrix.m03,boneInstance->localTransformMatrix.m13,boneInstance->localTransformMatrix.m23,result);		
		//boneInstance->worldTransformMatrix.m03 = result[0] - boneInstance->localTransformMatrix.m03 + boneInstance->nowPosX + parentInstance->worldTransformMatrix.m03;
		//boneInstance->worldTransformMatrix.m13 = result[1] - boneInstance->localTransformMatrix.m13 + boneInstance->nowPosY + parentInstance->worldTransformMatrix.m13;
		//boneInstance->worldTransformMatrix.m23 = result[2] - boneInstance->localTransformMatrix.m23 + boneInstance->nowPosZ + parentInstance->worldTransformMatrix.m23;
		MathQuaternionMultiplyVector3(&parentInstance->worldRot,boneInstance->localTransformMatrix.m03,
																boneInstance->localTransformMatrix.m13,
																boneInstance->localTransformMatrix.m23,
																result);
		boneInstance->worldTransformMatrix = MathQuaternionToMatrix(&boneInstance->worldRot);
		boneInstance->worldTransformMatrix.m03 = result[0]-boneInstance->localTransformMatrix.m03;
		boneInstance->worldTransformMatrix.m13 = result[1]-boneInstance->localTransformMatrix.m13;
		boneInstance->worldTransformMatrix.m23 = result[2]-boneInstance->localTransformMatrix.m23;*/
		boneInstance->worldRot = MathQuaternionMultiplyQuaternion(&parentInstance->worldRot,&boneInstance->localRot);
		boneInstance->worldTransformMatrix = MathQuaternionToMatrix(&boneInstance->localRot);
		boneInstance->worldTransformMatrix.m03 += boneInstance->nowPosX;
		boneInstance->worldTransformMatrix.m13 += boneInstance->nowPosY;
		boneInstance->worldTransformMatrix.m23 += boneInstance->nowPosZ;
	}
	else
	{
		//boneInstance->localTransformMatrix.m03 = 0;
		//boneInstance->localTransformMatrix.m13 = 0;
		//boneInstance->localTransformMatrix.m23 = 0;
		boneInstance->worldRot = boneInstance->localRot;
		boneInstance->worldTransformMatrix = MathQuaternionToMatrix(&boneInstance->worldRot);
		boneInstance->worldTransformMatrix = MathMatrixInvert(&boneInstance->worldTransformMatrix,NULL);
		boneInstance->worldTransformMatrix.m03 += boneInstance->nowPosX;
		boneInstance->worldTransformMatrix.m13 += boneInstance->nowPosY;
		boneInstance->worldTransformMatrix.m23 += boneInstance->nowPosZ;
		//rotMatrix = MathQuaternionToMatrix(&(boneInstance->localRot));
	}
	//rotMatrix = MathQuaternionToMatrix(&(boneInstance->localRot));
	//boneInstance->localTransformMatrix = MathMatrixMultiplyMatrix(&boneInstance->localTransformMatrix,&tempMatrix);
	//boneInstance->worldTransformMatrix = MathMatrixTranspose(&boneInstance->transformMatrix);
	//boneInstance->worldTransformMatrix = MathMatrixMultiplyMatrix(&tempMatrix,&boneInstance->worldTransformMatrix);
	//boneInstance->transformMatrix = MathMatrixTranspose(&boneInstance->transformMatrix);
	//boneInstance->transformMatrix = MathMatrixMultiplyMatrix(&tempMatrix,&boneInstance->transformMatrix);
	boneInstance->pass = 48;
}

void PMD_AnimationTick(PMD_ModelInstance *modelInstance)
{
	PMD_AnimationPlayer *player = &(modelInstance->animationPlayer);
	int i;
	if(player->animation==NULL)
		return;
	player->currentFrameIndex++;
	if(player->currentFrameIndex > player->animation->frameLength)
	{
		player->currentFrameIndex = 0;
		for(i=0;i<(modelInstance->model->boneCount);i++)
		{
			PMD_BoneInstance *boneInstance = modelInstance->boneHierarchy.boneInstaces + i;
			boneInstance->pass = 0;
			boneInstance->currentKeyFrame=boneInstance->firstKeyFrame;
			if(boneInstance->firstKeyFrame!=NULL)
			{
				boneInstance->nowPosX = boneInstance->firstKeyFrame->posX;
				boneInstance->nowPosY = boneInstance->firstKeyFrame->posY;
				boneInstance->nowPosZ = boneInstance->firstKeyFrame->posZ;
				MathMatrixLoadIdentity(&(boneInstance->localTransformMatrix));
				boneInstance->localTransformMatrix.m03 = boneInstance->firstKeyFrame->posX;
				boneInstance->localTransformMatrix.m13 = boneInstance->firstKeyFrame->posY;
				boneInstance->localTransformMatrix.m23 = boneInstance->firstKeyFrame->posZ;
				boneInstance->localRot = boneInstance->firstKeyFrame->rot;
			}
			else
			{
				boneInstance->nowPosX = 0.0f;
				boneInstance->nowPosY = 0.0f;
				boneInstance->nowPosZ = 0.0f;
				MathMatrixLoadIdentity(&(boneInstance->localTransformMatrix));
				MathQuaternionLoadIdentity(&(boneInstance->localRot));
			}
		}
	}
	else
	{
		for(i=0;i<(modelInstance->model->boneCount);i++)
		{
			PMD_BoneInstance *boneInstance = modelInstance->boneHierarchy.boneInstaces + i;
			boneInstance->pass = 0;
			if(boneInstance->currentKeyFrame!=NULL)
			{
				PMD_KeyFrame *currentKeyFrame = boneInstance->currentKeyFrame;
				PMD_KeyFrame *nextKeyFrame = currentKeyFrame->nextFrame;
				//Matrix tempMatrix;
				if(nextKeyFrame!=NULL)
				{
					if(player->currentFrameIndex == nextKeyFrame->currentFrameIndex)
					{
						currentKeyFrame = currentKeyFrame->nextFrame;
						boneInstance->currentKeyFrame = currentKeyFrame;
						boneInstance->nowPosX = currentKeyFrame->posX;
						boneInstance->nowPosY = currentKeyFrame->posY;
						boneInstance->nowPosZ = currentKeyFrame->posZ;
						MathMatrixLoadIdentity(&(boneInstance->localTransformMatrix));
						boneInstance->localTransformMatrix.m03 =currentKeyFrame->posX;
						boneInstance->localTransformMatrix.m13 = currentKeyFrame->posY;
						boneInstance->localTransformMatrix.m23 = currentKeyFrame->posZ;
						boneInstance->localRot = currentKeyFrame->rot;
					}
					else
					{
						float delta = (player->currentFrameIndex - currentKeyFrame->currentFrameIndex) / (float)(nextKeyFrame->currentFrameIndex - currentKeyFrame->currentFrameIndex);
						float delta2 = 1.0 - delta;
						boneInstance->nowPosX = nextKeyFrame->posX*delta + currentKeyFrame->posX*delta2;
						boneInstance->nowPosY = nextKeyFrame->posY*delta + currentKeyFrame->posY*delta2;
						boneInstance->nowPosZ = nextKeyFrame->posZ*delta + currentKeyFrame->posZ*delta2;
						MathMatrixLoadIdentity(&(boneInstance->localTransformMatrix));
						boneInstance->localTransformMatrix.m03 = boneInstance->nowPosX;
						boneInstance->localTransformMatrix.m13 = boneInstance->nowPosY;
						boneInstance->localTransformMatrix.m23 = boneInstance->nowPosZ;
						boneInstance->localRot = MathQuaternionSlerp(&currentKeyFrame->rot,&nextKeyFrame->rot,delta);
					}
					//boneInstance->pass = 48;
					boneInstance->pass |= 1; //标记为相对变换阶段
					//boneInstance->transformMatrix.m13 -= 1.4f;
					//tempMatrix = MathQuaternionToMatrix(&(boneInstance->localRot));
					//boneInstance->localTransformMatrix = MathMatrixMultiplyMatrix(&boneInstance->localTransformMatrix,&tempMatrix);
				}
			}
			else
			{
				/*boneInstance->nowPosX = 0.0f;
				boneInstance->nowPosY = 0.0f;
				boneInstance->nowPosZ = 0.0f;
				boneInstance->pass=0;
				MathMatrixLoadIdentity(&(boneInstance->transformMatrix));
				MathQuaternionLoadIdentity(&(boneInstance->nowRot));*/
				//DO NOTHING
			}
		}
	}
	for(i=0;i<(modelInstance->model->boneCount);i++)
	{
		PMD_BoneInstance *boneInstance = modelInstance->boneHierarchy.boneInstaces + i;
		if(boneInstance->pass!=48)
		{
			PMD_AnimationBoneCala(modelInstance,boneInstance);
		}
	}
	/*if(player->currentFrameIndex > player->animation->frameLength)
	{
		int i;
		player->currentFrame = 0;
		player->postFrame = 0;
		player->currentFrameIndex = 0;
		player->currentKeyFrame = player->animation->keyFrame;
		player->nextKeyFrame = player->animation->keyFrame+1;
		for(i=0;i<player->currentKeyFrame->count;i++)
		{
			PMD_BoneInstance *boneInstance = &modelInstance->boneHierarchy.boneInstaces[*((unsigned short *)HashTreeGet(modelInstance->model->boneMapping,player->currentKeyFrame->boneName[i],NULL))];
			boneInstance->pass |= 1; //标记为相对变换阶段
			boneInstance->nowPosX = player->currentKeyFrame->posX[i];
			boneInstance->nowPosY = player->currentKeyFrame->posY[i];
			boneInstance->nowPosZ = player->currentKeyFrame->posZ[i];
			boneInstance->nowRot = player->currentKeyFrame->rot[i];
			MathMatrixLoadIdentity(&(boneInstance->transformMatrix));
			boneInstance->transformMatrix.m03 = boneInstance->nowPosX;
			boneInstance->transformMatrix.m13 = boneInstance->nowPosY;
			boneInstance->transformMatrix.m23 = boneInstance->nowPosZ;
			boneInstance->transformMatrix = MathMatrixMultiplyMatrix(&boneInstance->transformMatrix,&MathQuaternionToMatrix(&(boneInstance->nowRot)));
		}
	}
	else
	{
		float rate = (float)(player->currentFrame - player->postFrame) / (float)(player->currentKeyFrame->nextFrame - player->postFrame);
		for(i=0;i<player->nextKeyFrame->count;i++)
		{
			PMD_BoneInstance *boneInstance = &modelInstance->boneHierarchy.boneInstaces[*((unsigned short *)HashTreeGet(modelInstance->model->boneMapping,player->nextKeyFrame->boneName[i],NULL))];
			boneInstance->pass |= 1; //标记为相对变换阶段
			boneInstance->nowPosX = player->currentKeyFrame->posX[i] + player->nextKeyFrame->posX[i]*rate;
			boneInstance->nowPosY = player->currentKeyFrame->posY[i] + player->nextKeyFrame->posY[i]*rate;
			boneInstance->nowPosZ = player->currentKeyFrame->posZ[i] + player->nextKeyFrame->posZ[i]*rate;
			boneInstance->nowRot = MathQuaternionSlerp(player->currentKeyFrame->rot+i,player->nextKeyFrame->rot+i,rate);
			MathMatrixLoadIdentity(&(boneInstance->transformMatrix));
			boneInstance->transformMatrix.m03 = boneInstance->nowPosX;
			boneInstance->transformMatrix.m13 = boneInstance->nowPosY;
			boneInstance->transformMatrix.m23 = boneInstance->nowPosZ;
			boneInstance->transformMatrix = MathMatrixMultiplyMatrix(&boneInstance->transformMatrix,&MathQuaternionToMatrix(&(boneInstance->nowRot)));
		}
		if(player->currentFrame==player->currentKeyFrame->nextFrame)
		{
			player->currentKeyFrame = player->nextKeyFrame;
			//player->nextKeyFrame = 
		}
	}*/
}
