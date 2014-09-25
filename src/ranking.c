#include "ranking.h"
#include <stdlib.h>
#include "memory.h"
#include <string.h>
#include <stdio.h>
#include "util.h"

static Rank *headRank = NULL;

void RankCreate(char *name,long long score)
{
	Rank *rank = (Rank*)malloc_s(sizeof(Rank));
	rank->name = (char*)malloc_s(sizeof(char)*(strlen(name)+1));
	strcpy(rank->name,name);
	rank->score = score;
	if(headRank==NULL)
	{
		headRank = rank;
		rank->nextRank = NULL;
	}
	else
	{
		char *tempC;
		long long tempL;
		Rank *temp = headRank;
		while((temp->score>score) && temp->nextRank!=NULL)
		{
			temp = temp->nextRank;
		}
		rank->nextRank = temp->nextRank;
		temp->nextRank = rank;
		tempL = rank->score;
		rank->score = temp->score;
		temp->score = tempL;
		tempC = rank->name;
		rank->name = temp->name;
		temp->name = tempC;
	}
}

void RankWriteOut()
{
	Rank *rank = NULL;
	FILE* file = fopen("ranking.txt","w");
	if(file==NULL)
	{
		LoggerError("Failed to write out ranking file.");
		return;
	}
	rank = headRank;
	while(rank!=NULL)
	{
		fprintf(file,"玩家:%s 分数:%ld\n",rank->name,rank->score);
		rank = rank->nextRank;
	}
	fclose(file);
}

void RankDestroy()
{
	Rank *rank = headRank;
	while(rank != NULL)
	{
		Rank *lastRank = rank;
		rank = rank->nextRank;
		free_s(lastRank->name);
		free_s(lastRank);
	}
	LoggerInfo("Ranking closed");
}