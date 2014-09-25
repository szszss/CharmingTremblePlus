#include "entity.h"
#include "memory.h"
#include "renderengine.h"
#include "resourcemanager.h"
#include "SDL_opengl.h"
#include "util.h"
#include "input.h"
#include "attribute.h"
#include <stdarg.h>
#include <stdlib.h>

EntityPrototype entityPlayerPrototype;
EntityBlockPrototype entityBlockPrototype;
EntityBlockPrototype entityBlockBrickPrototype;
EntityBlockPrototype entityBlockMossyPrototype;
EntityBlockPrototype entityBlockCobblestonePrototype;

extern unsigned long long tickTime;
extern World *theWorld;
extern Attribute attributeMossySlow;
extern PMD_Animation *animationRun;
extern PMD_Animation *animationStand;

int CallbackDestroyEntity( void* entity )
{
	EntityPrototype* p = ((Entity*)entity)->prototype;
	p->destroy(entity,theWorld,0);
	return 0;
}

void EntityDestroy(void* entity,World* world,int cause)
{
	Entity* ent = (Entity*)entity;
	LinkedListDestory(ent->attributeList,AttributeDestroyCallback);
	free_s(entity);
}

void* EntityPlayerCreate(World *world,float x,float y, ...)
{
	va_list args;
	EntityPlayer *player = (EntityPlayer*)malloc_s(sizeof(EntityPlayer));
	va_start(args,y);
	player->base.posX=x;
	player->base.posY=y;
	player->base.attributeList=LinkedListCreate();
	player->base.prototype=&entityPlayerPrototype;
	player->left=FALSE;
	player->right=FALSE;
	player->up=FALSE;
	player->down=FALSE;
	player->jump=FALSE;
	player->landed=FALSE;
	player->maxDepthLevel=0;
	player->speedX=0;
	player->speedY=0;
	player->speedFactorX=1.0f;
	player->speedFactorY=1.0f;
	player->life=5;
	player->score=0;
	//player->modelInstance=PMD_ModelInstanceCreate(PMD_LoadModel("model/koishi","koishi.pmd"));
	//PMD_UseAnimation(player->modelInstance,animationRun);
	player->modelInstance=NULL;
	player->id=va_arg(args, byte);
	va_end(args); 
	return player;
}

int EntityPlayerUpdate(void* entity,World* world)
{
	unsigned char operate;
	EntityPlayer *player = (EntityPlayer*)entity;
	char hTempMove = 0;
	//恢复参数
	player->speedFactorX=1.0f;
	player->speedFactorY=1.0f;

	AttributeUpdate(world,(Entity*)entity);

	//获取操作队列
	while((operate=IN_GetOperate())>200)
	{
		switch(operate)
		{
		case INPUT_OPERATE_LEFT_DOWN:
			hTempMove=(!hTempMove&&!player->left)?-1:0;
			player->left=TRUE;
			break;
		case INPUT_OPERATE_RIGHT_DOWN:
			hTempMove=(!hTempMove&&!player->right)?1:0;
			player->right=TRUE;
			break;
		case INPUT_OPERATE_UP_DOWN:
			player->up=TRUE;
			break;
		case INPUT_OPERATE_DOWN_DOWN:
			player->down=TRUE;
			break;
		case INPUT_OPERATE_SPACE_DOWN:
			player->jump=TRUE;
			break;
		case INPUT_OPERATE_LEFT_UP:
			player->left=FALSE;
			break;
		case INPUT_OPERATE_RIGHT_UP:
			player->right=FALSE;
			break;
		case INPUT_OPERATE_UP_UP:
			player->up=FALSE;
			break;
		case INPUT_OPERATE_DOWN_UP:
			player->down=FALSE;
			break;
		case INPUT_OPERATE_SPACE_UP:
			player->jump=FALSE;
			break;
		}
	}

	if(player->left||hTempMove<0)
	{
		player->speedX = -0.2f;
	}
	else if(player->right||hTempMove>0)
	{
		player->speedX = 0.2f;
	}
	else
	{
		player->speedX = 0.0f;
	}

	player->base.posX+=(player->speedX * player->speedFactorX);
	if(player->base.posX<-10.0f)
	{
		player->base.posX=-10.0f;
	}
	else if(player->base.posX>10.0f)
	{
		player->base.posX=10.0f;
	}

	if(player->landed)
	{
		//LoggerDebug("wwww");
		player->speedY=0.0f;
		if(player->jump)
		{
			player->speedY+=0.8f;
		}
	}
	else
	{
		player->base.posY+=player->speedY;
		player->speedY-=0.05f;
		
	}

	if(player->base.posY<-15)
	{
		if(player->speedY<0)
			EntityPlayerLifeChange(entity,world,-1);
		player->speedY = 1.5f;
	}
	else if(player->base.posY>14)
	{
		if(player->speedY>=0)
			EntityPlayerLifeChange(entity,world,-1);
		player->speedY = -0.1f;
		player->base.posY -= 2.5f;
	}

	if(player->speedY<-1.0f)
	{
		player->speedY=-1.0f;
	}
	player->landed=FALSE;
	return 0;
}

void EntityPlayerRender(void* entity,World* world)
{
	static Texture *texture = NULL;
	EntityPlayer *player = (EntityPlayer*)entity;
	glPushMatrix();
	glTranslatef(player->base.posX,player->base.posY,0);
	//glTranslatef(0,-10,20);
	if(player->modelInstance==NULL)
	{
		if(texture==NULL)
		{
			texture=RM_GetTexture("image/wood.png");
		}
		//glTranslatef(-4,3+(float)(tickTime)/10.0f,0);
		RE_BindTexture(texture);
		RE_ClearMaterial();
		RE_RenderCube(-0.5,2,-0.5,0.5,0,0.5);
		RE_BindTexture(NULL);
		
	}
	else
	{
		PMD_ModelInstanceRender(player->modelInstance);
	}
	glPopMatrix();
}

void EntityPlayerDestroy(void* entity,World* world,int cause)
{
	EntityPlayer *player = (EntityPlayer*)entity;
	PMD_ModelInstanceDestroy(player->modelInstance);
	EntityDestroy(entity,world,cause);
}

void EntityPlayerLifeChange( void* entity,World* world,int value )
{
	EntityPlayer *player = (EntityPlayer*)entity;
	player->life+=value;
	if(player->life<=0)
	{
		world->state=WSTATE_GAMEOVERING;
	}
}

void EntityBlockCreate_Do(World *world,float x,float y,EntityBlock *block,va_list args,char *texture,EntityPrototype *prototype)
{
	block->base.posX=x;
	block->base.posY=y;
	block->base.attributeList=LinkedListCreate();
	block->base.prototype=prototype;
	block->texture=RM_GetTexture(texture);
	block->stepped=0;
	block->width=va_arg(args, byte);
	block->depthLevel=va_arg(args, unsigned long);
}

void* EntityBlockCreate(World *world,float x,float y, ...)
{
	va_list args;
	EntityBlock *block = (EntityBlock*)malloc_s(sizeof(EntityBlock));
	va_start(args,y);
	EntityBlockCreate_Do(world,x,y,block,args,"image/stone.png",&entityBlockPrototype);
	va_end(args); 
	return block;
}

void* EntityBlockBrickCreate( World* world,float x,float y,... )
{
	va_list args;
	EntityBlock *block = (EntityBlock*)malloc_s(sizeof(EntityBlockBonus));
	va_start(args,y);
	EntityBlockCreate_Do(world,x,y,block,args,"image/brick.png",&entityBlockBrickPrototype);
	((EntityBlockBonus*)block)->bonusType=0;
	((EntityBlockBonus*)block)->bonusInNumber=0;
	((EntityBlockBonus*)block)->bounsInFactor=2.0f;
	((EntityBlockBonus*)block)->bounsPointer=NULL;
	va_end(args); 
	return block;
}

void* EntityBlockMossyCreate( World* world,float x,float y,... )
{
	va_list args;
	EntityBlock *block = (EntityBlock*)malloc_s(sizeof(EntityBlockBonus));
	va_start(args,y);
	EntityBlockCreate_Do(world,x,y,block,args,"image/mossy.png",&entityBlockMossyPrototype);
	((EntityBlockBonus*)block)->bonusType=0;
	((EntityBlockBonus*)block)->bonusInNumber=0;
	((EntityBlockBonus*)block)->bounsInFactor=2.0f;
	((EntityBlockBonus*)block)->bounsPointer=NULL;
	va_end(args); 
	return block;
}

int EntityBlockUpdate(void* entity,World* world)
{
	EntityBlock *block = (EntityBlock*)entity;
	float widthLeft,widthRight,temp;
	block->base.posY+=world->upSpeed;
	if(block->base.posY>20)
	{
		return -1;
	}
	AttributeUpdate(world,(Entity*)entity);
	temp=(float)(block->width)/2;
	widthLeft=block->base.posX-temp;
	widthRight=block->base.posX+temp;
	FOREACH_PLAYERS(player)
	if(player->base.posX>(widthLeft-0.2f) && player->base.posX<(widthRight+0.2f))
	{
		//LoggerDebug("yaya");
		if((player->base.posY > block->base.posY-1.0f) && (player->base.posY - block->base.posY < 0.7f) && (player->speedY<=0))
		{
			EntityBlockPrototype *prototype = block->base.prototype;
			//LoggerDebug("yyyyy");
			player->landed=TRUE;
			player->base.posY = block->base.posY+0.5f;
			if((unsigned long)(block->stepped&(1<<player->id))==0)//如果玩家第一次站上
			{
				prototype->onStep(entity,world,player,TRUE,0);
			}
			else
			{
				prototype->onStep(entity,world,player,FALSE,0); //TODO:正确的站立持续时间
			}
			//world->player->vSpeed=0;
		}
		else if(player->base.posY > block->base.posY-2.5f && (player->base.posY <= block->base.posY-0.7f))
		{
			if(player->speedY>0)
			{
				player->base.posY = block->base.posY-2.5f;
				player->speedY=0;
			}
			else
			{
				if(player->base.posX>block->base.posX)
					player->base.posX=widthRight+0.3f;
				else
					player->base.posX=widthLeft-0.3f;
			}
		}
	}
	FOREACH_END
	return 0;
}

void EntityBlockRender(void* entity,World* world)
{
	EntityBlock *block = (EntityBlock*)entity;
	int width = block->width;
	//float fWidth = (float)width/2.0f;;
	if(width<1)
	{
		LoggerDebug("A block has a wrong width:%d It won't be rendered",width);
		return;
	}
	glPushMatrix();

	RE_BindTexture(block->texture);
	RE_ClearMaterial();
	glTranslatef(block->base.posX,block->base.posY,0);
	RE_RenderCubeQuick(width);
	RE_BindTexture(NULL);
	glPopMatrix();
}

void EntityBlockOnStep( void* entity,World* world,EntityPlayer* player,BOOL first,int last )
{
	EntityBlock *block = (EntityBlock*)entity;
	long i = block->depthLevel - player->maxDepthLevel;
	if(first==TRUE)
	{
		if(i>0)
		{
			player->score += ( i - 1 )*10;
			player->maxDepthLevel=block->depthLevel;
		}
		else if(i<0)
		{
			player->score += -i*10;
		}
		player->score += 10;
		block->stepped |= (1<<player->id);
	}
	GameUpdateMaxScore(player->score);
}

void EntityBlockOnLeave( void* entity,World* world,EntityPlayer* player )
{
	//Do nothing
}

void EntityBlockOnStepMoreScore( void* entity,World* world,EntityPlayer* player,BOOL first,int last )
{
	EntityBlockBonus* block = (EntityBlockBonus*)entity;
	long long oldScore = player->score;
	long long delta = 0;
	EntityBlockOnStep(entity,world,player,first,last);
	delta = player->score - oldScore;
	if(delta!=0)
	{
		player->score -= delta;
		if(block->bonusType==0)
		{
			player->score += (long long)(delta*block->bounsInFactor)+block->bonusInNumber;
		}
		else
		{
			player->score += (long long)(delta+block->bonusInNumber)*block->bounsInFactor;
		}
	}
	GameUpdateMaxScore(player->score);
}

void EntityBlockOnStepSlow(void* entity,World* world,EntityPlayer* player,BOOL first,int last)
{
	EntityBlock* block = (EntityBlock*)entity;
	EntityBlockOnStep(entity,world,player,first,last);
	AttributeAddOrExtend(world,(Entity*)player,&attributeMossySlow);
}

void EntityBlockOnStepBreak(void* entity,World* world,EntityPlayer* player,BOOL first,int last)
{
	EntityBlockBonus* block = (EntityBlockBonus*)entity;
	EntityBlockOnStep(entity,world,player,first,last);
	if(first)
		block->bonusInNumber = 20;
}


int InitEntities()
{
	entityPlayerPrototype.create = EntityPlayerCreate;
	entityPlayerPrototype.update = EntityPlayerUpdate;
	entityPlayerPrototype.render = EntityPlayerRender;
	entityPlayerPrototype.destroy = EntityPlayerDestroy;
	((EntityPrototype*)&entityBlockPrototype)->create = EntityBlockCreate;
	((EntityPrototype*)&entityBlockPrototype)->update = EntityBlockUpdate;
	((EntityPrototype*)&entityBlockPrototype)->render = EntityBlockRender;
	((EntityPrototype*)&entityBlockPrototype)->destroy = EntityDestroy;
					  entityBlockPrototype.onStep = EntityBlockOnStep;
					  entityBlockPrototype.onLeave = EntityBlockOnLeave;
	entityBlockBrickPrototype=entityBlockPrototype;
	entityBlockBrickPrototype.base.create = EntityBlockBrickCreate;
	entityBlockBrickPrototype.onStep = EntityBlockOnStepMoreScore;
	entityBlockMossyPrototype=entityBlockPrototype;
	entityBlockMossyPrototype.base.create = EntityBlockMossyCreate;
	entityBlockMossyPrototype.onStep = EntityBlockOnStepSlow;
	//entityBlockCobblestonePrototype=entityBlockPrototype;
	//entityBlockCobblestonePrototype.base.create=EntityBlockCobblestoneCreate;
	LoggerInfo("Entities initialized");
	return 0;
}