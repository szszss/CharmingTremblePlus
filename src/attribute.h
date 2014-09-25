#pragma once
#include "game.h"

struct implAttribute
{
	/*当一个新的属性被添加到实体上时触发.返回值为是否同意添加.*/
	BOOL (*onAdd)(World*,Entity*,AttributeInstance*);
	/*当一个已有属性被延续时触发.返回值为是否同意延续.*/
	void (*onExtend)(World*,Entity*,AttributeInstance*);
	/*每tick触发一次.返回值为是否继续存在(返回FALSE立刻引发销毁和onExpire)*/
	BOOL (*onUpdate)(World*,Entity*,AttributeInstance*);
	/*每次3D渲染触发一次.*/
	void (*onRender)(World*,Entity*,AttributeInstance*);
	/*属性消除时引发.*/
	void (*onExpire)(World*,Entity*,AttributeInstance*);
	unsigned long defaultLife;
};

struct implAttributeInstance
{
	Attribute *attribute;
	/*剩余的Tick*/
	unsigned long lastLife;
	/*存在了多久(Tick)*/
	unsigned long elapseTime;
	long long dataBits;
};

int InitAttributes();

#define AttributeAdd(world,entity,attribute) AttributeAddOrExtend(world,entity,attribute)
BOOL AttributeAddOnly(World *world,Entity *entity,Attribute *attribute);
static BOOL AttributeAddDo(World *world,Entity *entity,Attribute *attribute);
BOOL AttributeAddOrExtend(World *world,Entity *entity,Attribute *attribute);
BOOL AttributeExtendOnly(World *world,Entity *entity,Attribute *attribute);
AttributeInstance* AttributeGet(Entity *entity,Attribute *attribute);
void AttributeUpdate(World *world,Entity *entity);
void AttributeRender(World *world,Entity *entity);
int AttributeDestroyCallback(void *attributeInstance);

static BOOL AttributeDefaultOnAdd(World *world,Entity *entity,AttributeInstance *attributeInstance);
static void AttributeDefaultOnExtend(World *world,Entity *entity,AttributeInstance *attributeInstance);
static BOOL AttributeDefaultOnUpdate(World *world,Entity *entity,AttributeInstance *attributeInstance);
static void AttributeDefaultOnRender(World *world,Entity *entity,AttributeInstance *attributeInstance);
static void AttributeDefaultOnExpire(World *world,Entity *entity,AttributeInstance *attributeInstance);
static BOOL AttributeMossySlowOnUpdate(World *world,Entity *entity,AttributeInstance *attributeInstance);

