#pragma once
#ifndef ranking_h__
#define ranking_h__

#include "game.h"
#include <string>

typedef struct implRank {
	Rank *nextRank;
	std::wstring name;
	long long score;
};

void RankCreate(const std::wstring & name,long long score);
void RankWriteOut();
void RankDestroy();

#endif // ranking_h__
