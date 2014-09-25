#pragma once
#include "game.h"
#include <wchar.h>

void OS_Init();
long long OS_GetMsTime();
char* OS_GetFontPath(char* fontName,char* backupFontName);
BOOL OS_PathExist(char* path);
BOOL OS_UTF8ToANSI_DO(char* utf8Text,wchar_t* destAnsiText,size_t maxLength);