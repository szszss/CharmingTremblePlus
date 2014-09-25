#ifndef util_h__
#define util_h__

#include "game.h"
#include <stdio.h>

/*
 *	梅森旋转算法随机数发生器
 */

struct implMTRandomGen
{
	long int MT[624];
	long int index ;
};

MTRandomGen* MTCreate(long seed);
long MTNext(MTRandomGen *randomGen);
long MTNextInt(MTRandomGen *randomGen,long min,long max);
void MTDestroy(MTRandomGen *randomGen);

/*
 *	日志记录器
 */

#define LOGGER_LEVEL_DEBUG 1
#define LOGGER_LEVEL_INFO 2
#define LOGGER_LEVEL_WARN 4
#define LOGGER_LEVEL_ERROR 8
#define LOGGER_LEVEL_FATAL 16
#define LOGGER_LEVEL_ALL 31
#define LOGGER_LEVEL_ALL_EXCEPT_DEBUG 30
#define LOGGER_LEVEL_NONE 0
#define LOGGER_FORMAT_C "%s: "
#define LOGGER_FORMAT_JAVA "[%s] "
enum LoggerMode {LOGGER_OVERRIDE,LOGGER_APPEND};

struct implLogger
{
	FILE* loggerFile;
	BOOL fileAvailable;
	enum LoggerMode mode;
	int level;
	char* format;
	int baseLength;
};

// Logger采用单例模式,程序中只能存在一个Logger
void LoggerCreate(BOOL logInFile,char* fileName,enum LoggerMode mode,int level,char* format);
int LoggerDebug(char* text,...);
//int LoggerDebugln(char* text);
int LoggerInfo(char* text,...);
//int LoggerInfoln(char* text);
int LoggerWarn(char* text,...);
//int LoggerWarnln(char* text);
int LoggerError(char* text,...);
//int LoggerErrorln(char* text);
int LoggerFatal(char* text,...);
//int LoggerFatalln(char* text);
void LoggerClose();

/*
 *	StringBuilder
 */

/*StringBuilder是一个能将多个字符串拼接为一个的工具.*/
#define STRING_BUILDER_MAX_BUFFER 24

typedef struct implStringBuilder{
	char** bufferedString;
	int bufferPointer;
};

/*创建一个新的StringBuilder.*/
StringBuilder* SBCreate();
/*向StringBuilder后追加一个字符串.*/
StringBuilder* SBAppend(StringBuilder *sb,char* str);
/*生成一个新字符串,生成完毕后会自动销毁StringBuilder.*/
char* SBBuild(StringBuilder *sb);
/*生成一个新字符串,生成完毕后保留StringBuilder.*/
char* SBBuildWithoutDestroy(StringBuilder *sb);
/*手动销毁一个StringBuilder,已Build的StringBuilder无需此步骤.*/
void SBDestroy(StringBuilder *sb);

/*
 *	字符串工具
 */

#define MAX_STRING_HASH 1000000000

Hash HashCode(char* string);

unsigned long* UTF8ToUTF32(char* utf8Text);
wchar_t* UTF8ToANSI(char* utf8Text);
//char* UTF32ToUTF8(unsigned long* utf32Text);

/*
 *	小端文件读取
 */

byte    LESReadByte(FILE *file);
void    LESReadBytes(FILE *file,byte *buffer,size_t count);
char    LESReadChar(FILE *file);
float   LESReadFloat(FILE *file);
double  LESReadDouble(FILE *file);
#define LESReadShort(f,ec) LESReadInt16(f,ec)
short   LESReadInt16(FILE *file);
long    LESReadInt32(FILE *file);
long long      LESReadInt64(FILE *file);
unsigned short LESReadUInt16(FILE *file);
unsigned long  LESReadUInt32(FILE *file);

#endif // util_h__