#pragma once
#include "game.h"
#include "collection.h"
#include "entity.h"
#include "util.h"

struct implWorld
{
	char *playerName;
	EntityPlayer *players[32];
	long seed;
	unsigned long tick;
	//unsigned long long score; //尽管玩到40亿分有些不太可能,但还是多多益善吧!别在乎那4byte的内存了.
	long depthLevel;
	double depth;
	LinkedList *blockList;
	LinkedList *powerupList;
	LinkedList *operateQueue;
	float upSpeed;
	enum WorldType type;
	enum WorldDifficulty difficulty;
	enum WorldState state;
	MTRandomGen *randomGen;
};

World* WorldNewGame(char* playerName,long seed,enum WorldType type,enum WorldDifficulty difficulty);
void WorldStart(World* world);
void WorldEnd(World* world);
void WorldUpdate(World* world);
void WorldRender(World* world);
void WorldDestory(World* world);
void WorldGameOver(World* world);