#include "attribute.h"
#include "entity.h"
#include "memory.h"
//#include "collection.h"

Attribute attributeTemplate;
Attribute attributeMossySlow;

int InitAttributes()
{
	attributeTemplate.onAdd = AttributeDefaultOnAdd;
	attributeTemplate.onExtend = AttributeDefaultOnExtend;
	attributeTemplate.onUpdate = AttributeDefaultOnUpdate;
	attributeTemplate.onRender = AttributeDefaultOnRender;
	attributeTemplate.onExpire = AttributeDefaultOnExpire;
	attributeTemplate.defaultLife = 0;
	attributeMossySlow = attributeTemplate;
	attributeMossySlow.onUpdate = AttributeMossySlowOnUpdate;
	attributeMossySlow.defaultLife = 300;
	LoggerInfo("Attributes initialized");
	return 0;
}

BOOL AttributeAddOnly(World *world,Entity *entity,Attribute *attribute)
{
	
	AttributeInstance *attributeInstance = AttributeGet(entity,attribute);
	if(attributeInstance!=NULL)
		return FALSE;
	return AttributeAddDo(world,entity,attribute);
}

static BOOL AttributeAddDo(World *world,Entity *entity,Attribute *attribute)
{
	BOOL shouldKeep;
	AttributeInstance *attributeInstance = (AttributeInstance*)malloc_s(sizeof(AttributeInstance));
	attributeInstance->attribute = attribute;
	attributeInstance->dataBits=0L;
	attributeInstance->elapseTime=0;
	attributeInstance->lastLife=0;
	shouldKeep = attribute->onAdd(world,entity,attributeInstance);
	if(!shouldKeep)
		free_s(attributeInstance);
	else
		LinkedListAdd(entity->attributeList,attributeInstance);
	return shouldKeep;
}

BOOL AttributeAddOrExtend(World *world,Entity *entity,Attribute *attribute)
{
	AttributeInstance *attributeInstance = AttributeGet(entity,attribute);
	if(attributeInstance!=NULL)
	{
		attribute->onExtend(world,entity,attributeInstance);
		return TRUE;
	}
	return AttributeAddDo(world,entity,attribute);
}

BOOL AttributeExtendOnly(World *world,Entity *entity,Attribute *attribute)
{
	AttributeInstance *attributeInstance = AttributeGet(entity,attribute);
	if(attributeInstance == NULL)
	{
		return FALSE;
	}
	attribute->onExtend(world,entity,attributeInstance);
	return TRUE;
}

AttributeInstance* AttributeGet(Entity *entity,Attribute *attribute)
{
	LinkedList *linkedList = entity->attributeList;
	LinkedListIterator *iterator = NULL;
	for(iterator=LinkedListGetIterator(linkedList);LinkedListIteratorHasNext(iterator);)
	{
		AttributeInstance *attributeInstance = (AttributeInstance*)LinkedListIteratorGetNext(iterator);
		if(attributeInstance->attribute==attribute)
		{
			return attributeInstance;
		}
	}
	return NULL;
}

void AttributeUpdate(World *world,Entity *entity)
{
	LinkedList *linkedList = entity->attributeList;
	LinkedListIterator *iterator = NULL;
	for(iterator=LinkedListGetIterator(linkedList);LinkedListIteratorHasNext(iterator);)
	{
		BOOL keepLive;
		AttributeInstance *attributeInstance = (AttributeInstance*)LinkedListIteratorGetNext(iterator);
		if(attributeInstance->lastLife==0)
			keepLive=FALSE;
		else
			keepLive = attributeInstance->attribute->onUpdate(world,entity,attributeInstance);
		if(!keepLive)
		{
			attributeInstance->attribute->onExpire(world,entity,attributeInstance);
			LinkedListIteratorDeleteCurrent(iterator);
			free_s(attributeInstance);
		}
		else
		{
			attributeInstance->lastLife--;
			attributeInstance->elapseTime++;
		}
	}
}

void AttributeRender(World *world,Entity *entity)
{
	LinkedList *linkedList = entity->attributeList;
	LinkedListIterator *iterator = NULL;
	for(iterator=LinkedListGetIterator(linkedList);LinkedListIteratorHasNext(iterator);)
	{
		BOOL keepLive;
		AttributeInstance *attributeInstance = (AttributeInstance*)LinkedListIteratorGetNext(iterator);
		attributeInstance->attribute->onRender(world,entity,attributeInstance);
	}
}

int AttributeDestroyCallback(void *attributeInstance)
{
	free_s(attributeInstance);
	return 0;
}

static BOOL AttributeDefaultOnAdd(World *world,Entity *entity,AttributeInstance *attributeInstance)
{
	attributeInstance->lastLife = attributeInstance->attribute->defaultLife;
	return TRUE;
}
static void AttributeDefaultOnExtend(World *world,Entity *entity,AttributeInstance *attributeInstance)
{
	attributeInstance->lastLife = attributeInstance->attribute->defaultLife;
}
static BOOL AttributeDefaultOnUpdate(World *world,Entity *entity,AttributeInstance *attributeInstance)
{
	return TRUE;
}
static void AttributeDefaultOnRender(World *world,Entity *entity,AttributeInstance *attributeInstance)
{
}
static void AttributeDefaultOnExpire(World *world,Entity *entity,AttributeInstance *attributeInstance)
{
}

static BOOL AttributeMossySlowOnUpdate(World *world,Entity *entity,AttributeInstance *attributeInstance)
{
	EntityPlayer* player = (EntityPlayer*)entity;
	if(player->landed && ((attributeInstance->lastLife) < (attributeInstance->attribute->defaultLife-5)))
		return FALSE;
	if(player->base.posY<-14.0f)
		return FALSE;
	player->speedFactorX *= 0.5f;
	//player->speedFactorY *= 0.75
	return TRUE;
}