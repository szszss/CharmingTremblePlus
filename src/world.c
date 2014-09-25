#include "world.h"
#include "memory.h"
#include "resourcemanager.h"
#include "ranking.h"

extern EntityPrototype entityPlayerPrototype;
extern EntityPrototype entityBlockPrototype;
extern EntityBlockPrototype entityBlockBrickPrototype;
extern EntityBlockPrototype entityBlockMossyPrototype;

World* WorldNewGame( char* playerName,long seed,enum WorldType type,enum WorldDifficulty difficulty )
{
	World *world = (World*)malloc_s(sizeof(World));
	int i;
	world->playerName=playerName;
	for(i=0;i<32;i++)
	{
		world->players[i] = NULL;
	}
	//world->score=0;
	world->seed=seed;
	world->tick=0;
	world->depth=0;
	world->depthLevel=0;
	world->type=type;
	world->difficulty=difficulty;
	world->state=WSTATE_STOP;
	LoggerInfo("A game world had been created");
	return world;
}

void WorldStart(World* world)
{
	EntityBlock* block = (EntityBlock*)entityBlockPrototype.create(world,0,-14,5,0);
	int i;
	for(i=0;i<32;i++)
	{
		world->players[i] = NULL;
	}
	world->tick=0;
	world->depth=0;
	world->depthLevel=0;
	world->upSpeed = 0.075f;
	world->powerupList=LinkedListCreate();
	world->blockList=LinkedListCreate();
	world->operateQueue=LinkedListCreate();
	world->randomGen=MTCreate(world->seed);

	block->stepped=0xFFFFFFFF;
	LinkedListAdd(world->blockList,block);
	world->players[0] = (EntityPlayer*)EntityPlayerCreate(world,0,-13,0);
	world->state=WSTATE_RUN;
	LoggerInfo("World started");
}

static BOOL _DummyDelete(void* v){return 0;}

void WorldEnd(World* world)
{
	if(world==NULL)
		return;
	LinkedListDestory(world->blockList,CallbackDestroyEntity);
	LinkedListDestory(world->powerupList,CallbackDestroyEntity);
	LinkedListDestory(world->operateQueue,_DummyDelete);
	FOREACH_PLAYERS(player)
		CallbackDestroyEntity(player);
	FOREACH_END
	MTDestroy(world->randomGen);
	world->state=WSTATE_STOP;
	LoggerInfo("World ended");
}

void WorldDestory(World* world)
{
	if(world==NULL)
		return;
	//free_s(world->player);
	LoggerInfo("Destroying world");
	free_s(world);
	LoggerInfo("World destroyed");
	//TODO:销毁操作队列
}

void UpdateEntityList(World* world,LinkedList *list)
{
	LinkedListIterator *iterator;
	void *entity;
	EntityPrototype* p;
	int result;
	for(iterator=LinkedListGetIterator(list);LinkedListIteratorHasNext(iterator);)
	{
		entity = LinkedListIteratorGetNext(iterator);
		p = ((Entity*)entity)->prototype;
		if((result=p->update(entity,world))<0)
		{
			p->destroy(entity,world,result);
			LinkedListIteratorDeleteCurrent(iterator);
		}
	}
}

void WorldUpdate( World* world )
{
	if(world->state==WSTATE_GAMEOVERING)
	{
		WorldGameOver(world);
	}
	if(world->state==WSTATE_RUN)
	{
		if(world->depth>5.0)
		{
			int count = MTNextInt(world->randomGen,0,1);
			int x = MTNextInt(world->randomGen,0,19);
			int blockType = MTNextInt(world->randomGen,0,10);
			byte length = (byte)MTNextInt(world->randomGen,4,10);
			EntityBlock *block = NULL;
			world->depth-=5.0;
			world->depthLevel++;
			switch(blockType)
			{
			case 0:
			case 1:
			case 3:
				block = (EntityBlock*)entityBlockBrickPrototype.base.create(world,(float)x-9.5f,-16,length,world->depthLevel);
				break;
			case 4:
			case 5:
				block = (EntityBlock*)entityBlockMossyPrototype.base.create(world,(float)x-9.5f,-16,length,world->depthLevel);
				break;
			default:
				block = (EntityBlock*)entityBlockPrototype.create(world,(float)x-9.5f,-16,length,world->depthLevel);
				break;
			}
			LinkedListAdd(world->blockList,block);
			world->upSpeed += 0.0003f;
		}
		UpdateEntityList(world,world->blockList);
		UpdateEntityList(world,world->powerupList);
		FOREACH_PLAYERS(player)
		((Entity*)(player))->prototype->update(player,world);
		FOREACH_END
	}
	world->depth+=world->upSpeed;
	world->tick++;
}

void RenderEntityList(World* world,LinkedList *list)
{
	LinkedListIterator *iterator;
	void *entity;
	EntityPrototype* p;
	for(iterator=LinkedListGetIterator(list);LinkedListIteratorHasNext(iterator);)
	{
		entity = LinkedListIteratorGetNext(iterator);
		p = ((Entity*)entity)->prototype;
		p->render(entity,world);
		//AttributeRender(world,(Entity*)entity);
	}
}

void WorldRender( World* world )
{
	if(world->state==WSTATE_RUN)
	{
		RenderEntityList(world,world->blockList);
		RenderEntityList(world,world->powerupList);
		FOREACH_PLAYERS(player)
		((Entity*)(player))->prototype->render(player,world);
		//AttributeRender(world,(Entity*)player);
		FOREACH_END
	}
	else if(world->state==WSTATE_GAMEOVERED)
	{
		//RE_DrawTextStatic("很遗憾,你♂死♂了",0.2,0.5,0.5);
		//RE_DrawTextStatic("hehe很遗憾,你♂死♂了",0.2,0.5,0.5);
	}
}

void WorldGameOver( World* world )
{
	world->state=WSTATE_GAMEOVERED;
	RankCreate(world->playerName,world->players[0]->score);
	RankWriteOut();
}
