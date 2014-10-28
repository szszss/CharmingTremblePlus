#pragma once
#include "game.h"
#include "world.h"
#include "pmd.h"

//#define FOREACH_PLAYERS FOREACH_PLAYERS(player)
#define FOREACH_PLAYERS(player,pWorld) {EntityPlayer *player = NULL;int _loopVar_=0;while((player=pWorld->players[_loopVar_++])!=NULL){
#define FOREACH_END     }}

//--------Entity
/*struct implEntityPrototype 
{
	void* (*create)(World&,float,float,...);
	int (*update)(void*,World&);
	void (*render)(void*,World&);
	void (*destroy)(void*,World&,int);
};*/
class Entity 
{
public:
	Entity(World& world,float x,float y);
	~Entity();
	LinkedList *attributeList;
	World *world;
	float posX;
	float posY;
	virtual int Update() { return 0; };
	virtual void Render() {};
	virtual void Destroy(int cause) {};
	virtual void AddAttribute(Attribute* attribute);
	virtual Attribute* GetAttribute(Attribute* attrInstance);
	virtual Attribute* GetAttribute(const type_info& attrClass);
protected:
	virtual void UpdateAttribute();
	virtual void RenderAttribute();
};
//--------Entity--EntityPlayer
class EntityPlayer : public Entity
{
public:
	EntityPlayer(World& world, float x, float y, byte playerId);
	byte id;
	int life;
	long long score; //尽管玩到40亿分有些不太可能,但还是多多益善吧!别在乎那4byte的内存了.
	float speedX;
	float speedY;
	float speedFactorX;
	float speedFactorY;
	float jumpFactor;
	long maxDepthLevel;
	BOOL left;
	BOOL right;
	BOOL up;
	BOOL down;
	BOOL jump;
	BOOL landed;
	PMD_ModelInstance *modelInstance;
	int Update();
	void Render();
	void Destroy(int cause);
	virtual void LifeChange(int value);
};
//--------Entity--EntityBlock
/*struct implEntityBlockPrototype 
{
	EntityPrototype base;
	void (*onStep)(void*,World&,EntityPlayer*,BOOL,int);
	void (*onLeave)(void*,World&,EntityPlayer*);
	//void* (*onBreak)(World&,EntityPlayer*);
};*/
class EntityBlock : public Entity
{
public:
	EntityBlock(World& world, float x, float y, byte width, unsigned long depth);
	long depthLevel;
	unsigned long stepped;
	Texture *texture;
	byte width;
	int Update();
	void Render();
	//void Destroy(int cause);
	virtual void OnStep(EntityPlayer& player, BOOL first, int last);
	virtual void OnLeave(EntityPlayer& player);
};
//--------Entity--EntityBlock-EntityBlockXXX
class EntityBlockMossy : public EntityBlock
{
public:
	EntityBlockMossy(World& world, float x, float y, byte width, unsigned long depth);
	void OnStep(EntityPlayer& player, BOOL first, int last);
protected:
	float slowFactor;
};
class EntityBlockBrick : public EntityBlock
{
public:
	EntityBlockBrick(World& world, float x, float y, byte width, unsigned long depth);
	void OnStep(EntityPlayer& player, BOOL first, int last);
protected:
	float bounsFactor;
};
//--------Entity--EntityPU
class EntityPU : public Entity
{
public:
	EntityPU(World& world, float x, float y) : Entity(world, x, y) {}
	int Update();
	virtual BOOL OnPick(EntityPlayer& player) { return FALSE; }
};

class EntityPUSpeed : public EntityPU
{
public:
	EntityPUSpeed(World& world, float x, float y) : EntityPU(world, x, y) {}
	void Render();
	BOOL OnPick(EntityPlayer& player);
};

class EntityPUJump : public EntityPU
{
public:
	EntityPUJump(World& world, float x, float y) : EntityPU(world, x, y) {}
	void Render();
	BOOL OnPick(EntityPlayer& player);
};

class EntityPULife : public EntityPU
{
public:
	EntityPULife(World& world, float x, float y) : EntityPU(world, x, y) {}
	void Render();
	BOOL OnPick(EntityPlayer& player);
};

int InitEntities();
int CallbackDestroyEntity(void* entity);