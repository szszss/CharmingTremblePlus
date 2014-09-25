#pragma once
#include "game.h"
#include "renderengine.h"

int RM_InitResourceManager();
int RM_Close();
Texture* RM_GetTexture(char* imageFile);
Texture* RM_LoadTexture(char* imageFile);