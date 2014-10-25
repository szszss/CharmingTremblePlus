#pragma once
#ifndef ranking_h__
#define ranking_h__

#include "game.h"
#include <wchar.h>

typedef struct implRank {
	Rank *nextRank;
	wchar_t* name;
	long long score;
};

void RankCreate(wchar_t* name,long long score);
void RankWriteOut();
void RankDestroy();

#endif // ranking_h__
