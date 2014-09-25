#pragma once
#ifndef ranking_h__
#define ranking_h__

#include "game.h"

typedef struct implRank {
	Rank *nextRank;
	char *name;
	long long score;
};

void RankCreate(char *name,long long score);
void RankWriteOut();
void RankDestroy();

#endif // ranking_h__
