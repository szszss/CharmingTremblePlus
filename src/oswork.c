#include "oswork.h"
#include "memory.h"
#include "util.h"
#include <locale.h>

#ifdef OS_WINDOWS
#pragma execution_character_set("utf-8")
//#pragma setlocale("english_england.UTF-8")
#include <Windows.h>

BOOL FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter);
BOOL FilePathExists(LPCTSTR lpPath);

char* OS_GetFontPath( char* fontName,char* backupFontName )
{
	//char hehe[] = _MSC_VER;
	char* fontFolder = getenv("WINDIR");
	int length = strlen(fontName)+strlen(fontFolder)+8;
	char* fontPath = (char*)malloc_s(length*sizeof(char));
	memset(fontPath,0,length);
	strcat(fontPath,fontFolder);
	strcat(fontPath,"\\fonts\\");
	strcat(fontPath,fontName);
	if(OS_PathExist(fontPath))
	{
		return fontPath;
	}
	else
	{
		if(backupFontName!=NULL)
		{
			return OS_GetFontPath(backupFontName,NULL);
		}
	}
	return NULL;
}

//Author:冰点青蛙
BOOL FindFirstFileExists(LPCTSTR lpPath, DWORD dwFilter)
{
	WIN32_FIND_DATA fd;
	HANDLE hFind = FindFirstFile(lpPath, &fd);
	BOOL bFilter = (FALSE == dwFilter) ? TRUE : fd.dwFileAttributes & dwFilter;
	BOOL RetValue = ((hFind != INVALID_HANDLE_VALUE) && bFilter) ? TRUE : FALSE;
	FindClose(hFind);
	return RetValue;
}

//Author:冰点青蛙
BOOL FilePathExists(LPCTSTR lpPath)
{
	return FindFirstFileExists(lpPath, FALSE);
}

BOOL OS_PathExist(char* path)
{
	unsigned long *buffer = UTF8ToUTF32(path);
	BOOL b = FilePathExists(path);
	free_s(buffer);
	return b;
}

long long OS_GetMsTime()
{
	return GetTickCount();
}

BOOL OS_UTF8ToANSI_DO( char* utf8Text, wchar_t* destAnsiText, size_t maxLength )
{
	return (mbstowcs(destAnsiText,utf8Text,maxLength)==maxLength)?FALSE:TRUE;
}

void OS_Init()
{
	setlocale(LC_ALL,".65001");
}


#endif