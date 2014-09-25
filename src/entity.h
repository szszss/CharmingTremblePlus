#pragma once
#include "game.h"
#include "world.h"
#include "pmd.h"

//#define FOREACH_PLAYERS FOREACH_PLAYERS(player)
#define FOREACH_PLAYERS(player) {EntityPlayer *player = NULL;int _loopVar_=0;while((player=world->players[_loopVar_++])!=NULL){
#define FOREACH_END     }}

//--------Entity
struct implEntityPrototype 
{
	void* (*create)(World*,float,float,...);
	int (*update)(void*,World*);
	void (*render)(void*,World*);
	void (*destroy)(void*,World*,int);
};
struct implEntity 
{
	EntityPrototype* prototype;
	LinkedList *attributeList;
	float posX;
	float posY;
};
//--------Entity--EntityBlock
struct implEntityBlockPrototype 
{
	EntityPrototype base;
	void (*onStep)(void*,World*,EntityPlayer*,BOOL,int);
	void (*onLeave)(void*,World*,EntityPlayer*);
	//void* (*onBreak)(World*,EntityPlayer*);
};
struct implEntityBlock 
{
	Entity base;
	long depthLevel;
	unsigned long stepped;
	Texture *texture;
	byte width;
};
//--------Entity--EntityBlock-EntityBlockBonus
struct implEntityBlockBonus 
{
	EntityBlock base;
	byte bonusType; //Believe me, this byte even DOSEN'T take space. Do you know why?
	int bonusInNumber;
	float bounsInFactor;
	void* bounsPointer;
};
//--------Entity--EntityPlayer
struct implEntityPlayer
{
	Entity base;
	byte id;
	int life;
	long long score; //尽管玩到40亿分有些不太可能,但还是多多益善吧!别在乎那4byte的内存了.
	float speedX;
	float speedY;
	float speedFactorX;
	float speedFactorY;
	long maxDepthLevel;
	BOOL left;
	BOOL right;
	BOOL up;
	BOOL down;
	BOOL jump;
	BOOL landed;
	PMD_ModelInstance *modelInstance;
};

int InitEntities();

void EntityDestroy(void* entity,World* world,int cause);
/*额外的附加参数:(byte)id - 玩家ID*/
void* EntityPlayerCreate(World* world,float x,float y,...);
int EntityPlayerUpdate(void* entity,World* world);
void EntityPlayerRender(void* entity,World* world);
void EntityPlayerDestroy(void* entity,World* world,int cause);
void EntityPlayerLifeChange(void* entity,World* world,int value);
/*额外的附加参数:(byte)width - 宽度,(uint32)depth - 深度*/
void* EntityBlockCreate(World* world,float x,float y,...);
int EntityBlockUpdate(void* entity,World* world);
void EntityBlockRender(void* entity,World* world);
void EntityBlockOnStep(void* entity,World* world,EntityPlayer* player,BOOL first,int last);
void EntityBlockOnLeave(void* entity,World* world,EntityPlayer* player);
/*额外分数砖块 使用附加值:bounsInFactor 分数加成系数 bonusInNumber 分数加值 bonusType 算法(0为加分=基准分*系数+加值 非0为加分=(基准分+加值)*系数)*/
void EntityBlockOnStepMoreScore(void* entity,World* world,EntityPlayer* player,BOOL first,int last);
/*踩上去后减速*/
void EntityBlockOnStepSlow(void* entity,World* world,EntityPlayer* player,BOOL first,int last);
/*踩上去后坏掉*/
void EntityBlockOnStepBreak(void* entity,World* world,EntityPlayer* player,BOOL first,int last);
/*额外的附加参数:(byte)width - 宽度,(uint32)depth - 深度*/
void* EntityBlockBrickCreate(World* world,float x,float y,...);
/*额外的附加参数:(byte)width - 宽度,(uint32)depth - 深度*/
void* EntityBlockMossyCreate(World* world,float x,float y,...);
/*额外的附加参数:(byte)width - 宽度,(uint32)depth - 深度*/
void* EntityBlockCobblestoneCreate(World* world,float x,float y,...);
int EntityBlockCobblestoneUpdate(void* entity,World* world);


int CallbackDestroyEntity(void* entity);