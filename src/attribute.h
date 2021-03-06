﻿#pragma once
#include "game.h"

class Attribute
{
public:
	Attribute() {lastLife = 0;  elapseTime = 0; dataBits = 0L;};
	/*当一个新的属性被添加到实体上时触发.返回值为是否同意添加.*/
	virtual BOOL OnAdd(World& world, Entity* entity) { lastLife = GetDefaultLife(); return TRUE; };
	/*当一个已有属性被延续时触发.*/
	virtual void OnExtend(World& world, Entity* entity) { lastLife = GetDefaultLife(); };
	/*每tick触发一次.返回值为是否继续存在(返回FALSE立刻引发销毁和onExpire)*/
	virtual BOOL OnUpdate(World& world, Entity* entity) { return TRUE; };
	/*每次3D渲染触发一次.*/
	virtual void OnRender(World& world, Entity* entity) {};
	/*属性消除时引发.*/
	virtual void OnExpire(World& world, Entity* entity) {};
	virtual int GetDefaultLife() { return 1; };
	long lastLife;
	long elapseTime;
	long long dataBits;
};

class AttributeMossySlow : public Attribute
{
public:
	BOOL OnUpdate(World& world, Entity* entity);
	int GetDefaultLife();
};

class AttributePowerUpSpeed : public Attribute
{
public:
	BOOL OnUpdate(World& world, Entity* entity);
	int GetDefaultLife();
};

class AttributePowerUpJump : public Attribute
{
public:
	BOOL OnUpdate(World& world, Entity* entity);
	int GetDefaultLife();
};

int InitAttributes();
int AttributeDestroyCallback(void *attributeInstance);

