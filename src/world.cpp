#include "world.h"
#include "memory.h"
#include "resourcemanager.h"
#include "ranking.h"

/*extern EntityPrototype entityPlayerPrototype;
extern EntityBlockPrototype entityBlockPrototype;
extern EntityBlockPrototype entityBlockBrickPrototype;
extern EntityBlockPrototype entityBlockMossyPrototype;*/

World::World(wchar_t* playerName, long seed, enum WorldType type, enum WorldDifficulty difficulty)
{
	//World &world = (World&)malloc_s(sizeof(World));
	int i;
	this->playerName=playerName;
	for(i=0;i<32;i++)
	{
		players[i] = NULL;
	}
	//world->score=0;
	this->seed=seed;
	tick=0;
	depth=0;
	depthLevel=0;
	this->type=type;
	this->difficulty=difficulty;
	state=WSTATE_STOP;
	LoggerInfo("A game world had been created");
	//return world;
}

void World::Start()
{
	EntityBlock* block = new EntityBlock(*this,0,-14,5,0);
	int i;
	for(i=0;i<32;i++)
	{
		players[i] = NULL;
	}
	tick=0;
	depth=0;
	depthLevel=0;
	upSpeed = 0.075f;
	powerupList=LinkedListCreate();
	blockList=LinkedListCreate();
	operateQueue=LinkedListCreate();
	randomGen=MTCreate(seed);

	block->stepped=0xFFFFFFFF;
	LinkedListAdd(blockList,block);
	this->players[0] = new EntityPlayer(*this,0,-13,0);
	this->state=WSTATE_RUN;
	LoggerInfo("World started");
}

static BOOL _DummyDelete(void* v){return 0;}

void World::End()
{
	World &world = *this;
	//if(world==NULL)
	//	return;
	LinkedListDestory(blockList,CallbackDestroyEntity);
	LinkedListDestory(powerupList,CallbackDestroyEntity);
	LinkedListDestory(operateQueue,_DummyDelete);
	FOREACH_PLAYERS(player,this)
		CallbackDestroyEntity(player);
	FOREACH_END
	MTDestroy(randomGen);
	state=WSTATE_STOP;
	LoggerInfo("World ended");
}

void World::Destory()
{
	//if(world==NULL)
	//	return;
	//free_s(world->player);
	LoggerInfo("Destroying world");
	//delete(this);
	//free_s(world);
	LoggerInfo("World destroyed");
	//TODO:销毁操作队列
}

void World::UpdateEntityList(LinkedList *list)
{
	LinkedListIterator *iterator;
	Entity *entity;
	int result;
	for(iterator=LinkedListGetIterator(list);LinkedListIteratorHasNext(iterator);)
	{
		entity = (Entity*)LinkedListIteratorGetNext(iterator);
		if((result=entity->Update())<0)
		{
			entity->Destroy(result);
			delete(entity);
			LinkedListIteratorDeleteCurrent(iterator);
		}
	}
}

void World::Update()
{
	if(state==WSTATE_GAMEOVERING)
	{
		GameOver();
	}
	if(state==WSTATE_RUN)
	{
		if(depth>5.0)
		{
			int count = MTNextInt(randomGen,0,1);
			int x = MTNextInt(randomGen,0,19);
			int blockType = MTNextInt(randomGen,0,10);
			byte length = (byte)MTNextInt(randomGen,4,10);
			int hasPower = (byte)MTNextInt(randomGen,0,9);
			EntityBlock *block = NULL;
			depth-=5.0;
			depthLevel++;
			switch(blockType)
			{
			case 0:
			case 1:
			case 3:
				block = new EntityBlockBrick(*this,(float)x-9.5f,-16,length,depthLevel);
				break;
			case 4:
			case 5:
				block = new EntityBlockMossy(*this, (float)x - 9.5f, -16, length, depthLevel);
				break;
			default:
				block = new EntityBlock(*this, (float)x - 9.5f, -16, length, depthLevel);
				break;
			}
			LinkedListAdd(blockList,block);
			EntityPU *pu = NULL;
			x = MTNextInt(randomGen,0,19);
			switch(hasPower)
			{
			case 0:
				pu = new EntityPULife(*this, (float)x - 9.5f, -19);
				break;
			case 1:
			case 2:
				pu = new EntityPUSpeed(*this, (float)x - 9.5f, -19);
				break;
			case 3:
			case 4:
				pu = new EntityPUJump(*this, (float)x - 9.5f, -19);
				break;
			default:
				break;
			}
			if(pu!=NULL)
				LinkedListAdd(powerupList,pu);
			upSpeed += 0.0003f;
		}
		UpdateEntityList(blockList);
		UpdateEntityList(powerupList);
		FOREACH_PLAYERS(player, this)
			player->Update();
		FOREACH_END
	}
	depth+=upSpeed;
	tick++;
}

void World::RenderEntityList(LinkedList *list)
{
	LinkedListIterator *iterator;
	Entity *entity;
	for(iterator=LinkedListGetIterator(list);LinkedListIteratorHasNext(iterator);)
	{
		entity = (Entity*)LinkedListIteratorGetNext(iterator);
		entity->Render();
		//AttributeRender(world,(Entity*)entity);
	}
}

void World::Render()
{
	if(state==WSTATE_RUN)
	{
		RenderEntityList(blockList);
		RenderEntityList(powerupList);
		FOREACH_PLAYERS(player, this)
			player->Render();
		//AttributeRender(world,(Entity*)player);
		FOREACH_END
	}
	else if(state==WSTATE_GAMEOVERED)
	{
		//RE_DrawTextStatic("很遗憾,你♂死♂了",0.2,0.5,0.5);
		//RE_DrawTextStatic("hehe很遗憾,你♂死♂了",0.2,0.5,0.5);
	}
}

void World::GameOver()
{
	state=WSTATE_GAMEOVERED;
	RankCreate(playerName==NULL?L"":playerName,players[0]->score);
	RankWriteOut();
}
