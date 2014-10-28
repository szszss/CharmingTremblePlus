#include "attribute.h"
#include "entity.h"
#include "memory.h"
//#include "collection.h"

int InitAttributes()
{
	LoggerInfo("Attributes initialized");
	return 0;
}

BOOL AttributeMossySlow::OnUpdate(World &world, Entity *entity)
{
	EntityPlayer* player = (EntityPlayer*)entity;
	if (player->landed && ((lastLife) < (GetDefaultLife() - 5)))
		return FALSE;
	if (player->posY<-14.0f)
		return FALSE;
	player->speedFactorX *= 0.5f;
	return TRUE;
}

int AttributeMossySlow::GetDefaultLife()
{
	return 300;
}

BOOL AttributePowerUpSpeed::OnUpdate(World &world, Entity *entity)
{
	EntityPlayer* player = (EntityPlayer*)entity;
	player->speedFactorX *= 2.0f;
	return TRUE;
}

int AttributePowerUpSpeed::GetDefaultLife()
{
	return 300;
}

BOOL AttributePowerUpJump::OnUpdate(World &world, Entity *entity)
{
	EntityPlayer* player = (EntityPlayer*)entity;
	player->jumpFactor *= 1.5f;
	return TRUE;
}

int AttributePowerUpJump::GetDefaultLife()
{
	return 300;
}

int AttributeDestroyCallback(void *attributeInstance)
{
	delete(attributeInstance);
	return 0;
}