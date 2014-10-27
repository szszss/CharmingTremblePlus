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
	//player->speedFactorY *= 0.75
	return TRUE;
}


int AttributeDestroyCallback(void *attributeInstance)
{
	delete(attributeInstance);
	return 0;
}