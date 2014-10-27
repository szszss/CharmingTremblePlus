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
#include <typeinfo>

/*EntityPrototype entityPlayerPrototype;
EntityBlockPrototype entityBlockPrototype;
EntityBlockPrototype entityBlockBrickPrototype;
EntityBlockPrototype entityBlockMossyPrototype;
EntityBlockPrototype entityBlockCobblestonePrototype;*/

extern unsigned long long tickTime;
extern World *theWorld;
extern Attribute attributeMossySlow;
extern PMD_Animation *animationRun;
extern PMD_Animation *animationStand;

int CallbackDestroyEntity( void* entity )
{
	Entity* p = ((Entity*)entity);
	p->Destroy(0);
	return 0;
}

/*void EntityDestroy(void* entity,World& world,int cause)
{
	Entity* ent = (Entity*)entity;
	LinkedListDestory(ent->attributeList,AttributeDestroyCallback);
	free_s(entity);
}*/

Entity::Entity(World& world, float x, float y)
{
	this->world = &world;
	this->posX = x;
	this->posY = y;
	this->attributeList = LinkedListCreate();
}

Entity::~Entity()
{
	LinkedListDestory(this->attributeList, AttributeDestroyCallback);
}

void Entity::AddAttribute(Attribute* attribute)
{
	Attribute *attr = GetAttribute(typeid(attribute));
	if (attr != NULL)
	{
		*attr = *attribute;
		attr->OnExtend(*world, this);
		delete(attribute);
	}
	else
	{
		BOOL shouldKeep = attribute->OnAdd(*world, this);
		if (shouldKeep)
			LinkedListAdd(attributeList, attribute);
		else
			delete(attribute);
	}
}

Attribute* Entity::GetAttribute(const type_info& attrClass)
{
	LinkedList* linkedList = attributeList;
	LinkedListIterator *iterator = NULL;
	for (iterator = LinkedListGetIterator(linkedList); LinkedListIteratorHasNext(iterator);)
	{
		Attribute *attribute = (Attribute*)LinkedListIteratorGetNext(iterator);
		if (attrClass == typeid(attribute))
		{
			return attribute;
		}
	}
	return NULL;
}

void Entity::UpdateAttribute()
{
	LinkedList *linkedList = this->attributeList;
	LinkedListIterator *iterator = NULL;
	for (iterator = LinkedListGetIterator(linkedList); LinkedListIteratorHasNext(iterator);)
	{
		BOOL keepLive;
		Attribute *attribute = (Attribute*)LinkedListIteratorGetNext(iterator);
		if (attribute->lastLife == 0)
			keepLive = FALSE;
		else
			keepLive = attribute->OnUpdate(*world, this);
		if (!keepLive)
		{
			attribute->OnExpire(*world, this);
			LinkedListIteratorDeleteCurrent(iterator);
			delete(attribute);
		}
		else
		{
			attribute->lastLife--;
			attribute->elapseTime++;
		}
	}
}

void Entity::RenderAttribute()
{
	LinkedList *linkedList = this->attributeList;
	LinkedListIterator *iterator = NULL;
	for (iterator = LinkedListGetIterator(linkedList); LinkedListIteratorHasNext(iterator);)
	{
		Attribute *attribute = (Attribute*)LinkedListIteratorGetNext(iterator);
		attribute->OnRender(*world, this);
	}
}

EntityPlayer::EntityPlayer(World& world, float x, float y, byte playerId) : Entity(world, x, y)
{
	this->id = playerId;
	this->life = 5;
	this->score = 0;
	this->speedX = 0;
	this->speedY = 0;
	this->speedFactorX = 1;
	this->speedFactorY = 1;
	this->maxDepthLevel = 0;
	this->left = FALSE;
	this->right = FALSE;
	this->up = FALSE;
	this->down = FALSE;
	this->jump = FALSE;
	this->landed = FALSE;
	this->modelInstance = NULL;
}

int EntityPlayer::Update()
{
	unsigned char operate;
	char hTempMove = 0;
	//恢复参数
	this->speedFactorX=1.0f;
	this->speedFactorY = 1.0f;

	UpdateAttribute();

	//获取操作队列
	while((operate=IN_GetOperate())>200)
	{
		switch(operate)
		{
		case INPUT_OPERATE_LEFT_DOWN:
			hTempMove = (!hTempMove&&!this->left) ? -1 : 0;
			this->left = TRUE;
			break;
		case INPUT_OPERATE_RIGHT_DOWN:
			hTempMove = (!hTempMove&&!this->right) ? 1 : 0;
			this->right = TRUE;
			break;
		case INPUT_OPERATE_UP_DOWN:
			this->up = TRUE;
			break;
		case INPUT_OPERATE_DOWN_DOWN:
			this->down = TRUE;
			break;
		case INPUT_OPERATE_SPACE_DOWN:
			this->jump = TRUE;
			break;
		case INPUT_OPERATE_LEFT_UP:
			this->left = FALSE;
			break;
		case INPUT_OPERATE_RIGHT_UP:
			this->right = FALSE;
			break;
		case INPUT_OPERATE_UP_UP:
			this->up = FALSE;
			break;
		case INPUT_OPERATE_DOWN_UP:
			this->down = FALSE;
			break;
		case INPUT_OPERATE_SPACE_UP:
			this->jump = FALSE;
			break;
		}
	}

	if (this->left || hTempMove<0)
	{
		this->speedX = -0.2f;
	}
	else if (this->right || hTempMove>0)
	{
		this->speedX = 0.2f;
	}
	else
	{
		this->speedX = 0.0f;
	}

	this->posX += (this->speedX * this->speedFactorX);
	if (this->posX<-10.0f)
	{
		this->posX = -10.0f;
	}
	else if (this->posX>10.0f)
	{
		this->posX = 10.0f;
	}

	if (this->landed)
	{
		//LoggerDebug("wwww");
		this->speedY = 0.0f;
		if (this->jump)
		{
			this->speedY += 0.8f;
		}
	}
	else
	{
		this->posY += this->speedY;
		this->speedY -= 0.05f;
		
	}

	if (this->posY<-15)
	{
		if (this->speedY<0)
			LifeChange(-1);
		this->speedY = 1.5f;
	}
	else if (this->posY>14)
	{
		if (this->speedY >= 0)
			LifeChange(-1);
		this->speedY = -0.1f;
		this->posY -= 2.5f;
	}

	if (this->speedY<-1.0f)
	{
		this->speedY = -1.0f;
	}
	this->landed = FALSE;
	return 0;
}

void EntityPlayer::LifeChange(int value)
{
	this->life += value;
	if (this->life <= 0)
	{
		world->state = WSTATE_GAMEOVERING;
	}
}

void EntityPlayer::Render()
{
	static Texture *texture = NULL;
	EntityPlayer *player = this;
	glPushMatrix();
	glTranslatef(player->posX,player->posY,0);
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
	RenderAttribute();
	glPopMatrix();
}

void EntityPlayer::Destroy(int cause)
{
	PMD_ModelInstanceDestroy(this->modelInstance);
}

EntityBlock::EntityBlock(World& world, float x, float y, byte width, unsigned long depth) : Entity(world, x, y)
{
	this->texture = GetTexture();
	this->stepped = 0;
	this->width = width;
	this->depthLevel = depth;
}

Texture* EntityBlock::GetTexture()
{
	return RM_GetTexture("image/stone.png");
}

EntityBlockBrick::EntityBlockBrick(World& world, float x, float y, byte width, unsigned long depth) :
						EntityBlock(world, x, y, width, depth)
{
	this->bounsFactor = 2.0f;
}

Texture* EntityBlockBrick::GetTexture()
{
	return RM_GetTexture("image/brick.png");
}

EntityBlockMossy::EntityBlockMossy(World& world, float x, float y, byte width, unsigned long depth) :
						EntityBlock(world, x, y, width, depth)
{
	this->slowFactor = 2.0f;
}

Texture* EntityBlockMossy::GetTexture()
{
	return RM_GetTexture("image/mossy.png");
}

int EntityBlock::Update()
{
	EntityBlock *block = this;
	float widthLeft,widthRight,temp;
	block->posY+=world->upSpeed;
	if(block->posY>20)
	{
		return -1;
	}
	UpdateAttribute();
	temp=(float)(block->width)/2;
	widthLeft=block->posX-temp;
	widthRight=block->posX+temp;
	FOREACH_PLAYERS(player,world)
	if(player->posX>(widthLeft-0.2f) && player->posX<(widthRight+0.2f))
	{
		//LoggerDebug("yaya");
		if((player->posY > block->posY-1.0f) && (player->posY - block->posY < 0.7f) && (player->speedY<=0))
		{
			//EntityBlockPrototype *prototype = (EntityBlockPrototype*)block->base.prototype;
			//LoggerDebug("yyyyy");
			player->landed=TRUE;
			player->posY = block->posY+0.5f;
			if((unsigned long)(block->stepped&(1<<player->id))==0)//如果玩家第一次站上
			{
				OnStep(*player, TRUE, 0);
			}
			else
			{
				OnStep(*player, FALSE, 0);//TODO:正确的站立持续时间
			}
			//world->player->vSpeed=0;
		}
		else if(player->posY > block->posY-2.5f && (player->posY <= block->posY-0.7f))
		{
			if(player->speedY>0)
			{
				player->posY = block->posY-2.5f;
				player->speedY=0;
			}
			else
			{
				if(player->posX>block->posX)
					player->posX=widthRight+0.3f;
				else
					player->posX=widthLeft-0.3f;
			}
		}
	}
	FOREACH_END
	return 0;
}

void EntityBlock::Render()
{
	EntityBlock *block = this;
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
	glTranslatef(block->posX,block->posY,0);
	RE_RenderCubeQuick(width);
	RE_BindTexture(NULL);
	RenderAttribute();
	glPopMatrix();
}

void EntityBlock::OnStep(EntityPlayer& player,BOOL first,int last )
{
	EntityBlock *block = this;
	long i = block->depthLevel - player.maxDepthLevel;
	if(first==TRUE)
	{
		if(i>0)
		{
			player.score += ( i - 1 )*10;
			player.maxDepthLevel=block->depthLevel;
		}
		else if(i<0)
		{
			player.score += -i*10;
		}
		player.score += 10;
		block->stepped |= (1<<player.id);
	}
	GameUpdateMaxScore(player.score);
}

void EntityBlock::OnLeave( EntityPlayer& player )
{
	//Do nothing
}

void EntityBlockBrick::OnStep( EntityPlayer& player,BOOL first,int last )
{
	EntityBlockBrick* block = this;
	long long oldScore = player.score;
	long long delta = 0;
	EntityBlock::OnStep(player,first,last);
	delta = player.score - oldScore;
	if(delta!=0)
	{
		player.score -= delta;
		player.score += (long long)(delta*bounsFactor);
	}
	GameUpdateMaxScore(player.score);
}

void EntityBlockMossy::OnStep(EntityPlayer& player,BOOL first,int last)
{
	EntityBlockMossy* block = this;
	EntityBlock::OnStep(player, first, last);
	player.AddAttribute(new AttributeMossySlow());
}

/*void EntityBlockOnStepBreak(void* entity,World& world,EntityPlayer* player,BOOL first,int last)
{
	EntityBlockBonus* block = (EntityBlockBonus*)entity;
	EntityBlockOnStep(entity,world,player,first,last);
	if(first)
		block->bonusInNumber = 20;
}*/


int InitEntities()
{
	/*entityPlayerPrototype.create = EntityPlayerCreate;
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
	entityBlockMossyPrototype.onStep = EntityBlockOnStepSlow;*/
	//entityBlockCobblestonePrototype=entityBlockPrototype;
	//entityBlockCobblestonePrototype.base.create=EntityBlockCobblestoneCreate;
	LoggerInfo("Entities initialized");
	return 0;
}