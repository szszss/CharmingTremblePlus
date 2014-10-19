#include "util.h"
#include "oswork.h"
#include "memory.h"
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>

//梅森旋转算法随机数发生器

MTRandomGen* MTCreate(long seed)
{
	int i;
	MTRandomGen *randomGen = (MTRandomGen*)malloc_s(sizeof(MTRandomGen));
	randomGen->MT[0]=seed;
	randomGen->index=0;
	for (i = 1; i < 624; i++) {
		randomGen->MT[i] = 0xffffffff & (0x6c078965 * (randomGen->MT[i - 1] ^ (randomGen->MT[i - 1] >> 30)) + i);
	}
	return randomGen;
}

void MTRegenerate(MTRandomGen *randomGen)
{
	int i,y;
	for (i = 0; i < 624; i++)
	{
		y = (randomGen->MT[i] & 0x80000000) + (randomGen->MT[(i + 1) % 624] & 0x7fffffff);
		randomGen->MT[i] = randomGen->MT[(i + 397) % 624] ^ (y >> 1);
		if (y % 2 != 0)
		{
			randomGen->MT[i] ^= 0x9908b0df;
		}
	}
}

long MTNext(MTRandomGen *randomGen)
{
	int y;
	if (randomGen->index == 0)
	{
		MTRegenerate(randomGen);
	}
	y = randomGen->MT[randomGen->index];
	y ^= (y >> 11);
	y ^= ((y << 7) & 0x9d2c5680);
	y ^= ((y << 15) & 0xefc60000);
	y ^= (y >> 18);
	randomGen->index = (randomGen->index + 1) % 624;
	return y;
}

long MTNextInt(MTRandomGen *randomGen,long min,long max)
{
	if(max<min)
	{
		return MTNextInt(randomGen,max,min);
	}
	return MTNext(randomGen) % (max - min + 1) + min;
}

void MTDestroy(MTRandomGen *randomGen)
{
	free_s(randomGen);
}


Logger logger;

void LoggerCreate(BOOL logInFile,char* fileName,enum LoggerMode mode,int level,char* format)
{
	logger.fileAvailable = logInFile;
	logger.level=level;
	logger.mode=mode;
	logger.format=format;
	logger.baseLength=strlen(format)+2;
	if(logger.fileAvailable)
	{
		switch(logger.mode)
		{
		case LOGGER_OVERRIDE:
			logger.loggerFile = fopen(fileName,"w");
			break;
		case LOGGER_APPEND:
			logger.loggerFile = fopen(fileName,"a+");
			break;
		default:
			logger.fileAvailable=FALSE;
			return;
		}
		//检查日志文件是否打开成功
		if(logger.loggerFile == NULL)
		{
			logger.fileAvailable=FALSE;
			LoggerError("Created/Opened log file failed. Logger will still run without log file");
		}
		LoggerInfo("Logger initialized. Hello Yooooorld!");
	}
}
int LoggerOutput(char* level,char* text,va_list args)
{
	printf(logger.format,level);
	vprintf(text,args);
	printf("\n");
	if(logger.fileAvailable)
	{
		fprintf(logger.loggerFile,logger.format,level);
		vfprintf(logger.loggerFile,text,args);
		fprintf(logger.loggerFile,"\n");
		return 2;
	}
	return 1;
}

int LoggerDebug(char* text,...)
{
	va_list args;
	va_start(args, text);
	if(logger.level&LOGGER_LEVEL_DEBUG)
		return LoggerOutput("Debug",text,args);
	va_end(args);
	return 0;
}
//int LoggerDebugln(char* text);
int LoggerInfo(char* text,...)
{
	va_list args;
	va_start(args, text);
	if(logger.level&LOGGER_LEVEL_INFO)
		return LoggerOutput("Info",text,args);
	va_end(args);
	return 0;
}
//int LoggerInfoln(char* text);
int LoggerWarn(char* text,...)
{
	va_list args;
	va_start(args, text);
	if(logger.level&LOGGER_LEVEL_WARN)
		return LoggerOutput("Warn",text,args);
	va_end(args);
	return 0;
}
//int LoggerWarnln(char* text);
int LoggerError(char* text,...)
{
	va_list args;
	va_start(args, text);
	if(logger.level&LOGGER_LEVEL_ERROR)
		return LoggerOutput("Error",text,args);
	va_end(args);
	return 0;
}
//int LoggerErrorln(char* text);
int LoggerFatal(char* text,...)
{
	va_list args;
	int result = 0;
	va_start(args, text);
	if(logger.level&LOGGER_LEVEL_FATAL)
		result = LoggerOutput("Fatal",text,args);
	va_end(args);
	return result;
}
//int LoggerFatalln(char* text);
void LoggerClose()
{
	LoggerInfo("Closing logger. Remained pointers:%d. Good-byte! Cruel world!\n",GetRemainedPointerCount());
	fclose(logger.loggerFile);
}

StringBuilder* SBCreate()
{
	StringBuilder *sb = (StringBuilder*)malloc_s(sizeof(StringBuilder));
	sb->bufferedString = (char**)malloc_s(sizeof(char*)*STRING_BUILDER_MAX_BUFFER);
	sb->bufferPointer = 0;
	return sb;
}

StringBuilder* SBAppend( StringBuilder *sb,char* str )
{
	if(sb->bufferPointer==STRING_BUILDER_MAX_BUFFER)
		return sb;
	sb->bufferedString[sb->bufferPointer]=str;
	sb->bufferPointer++;
	return sb;
}

char* SBBuild( StringBuilder *sb )
{
	char* str=SBBuildWithoutDestroy(sb);
	SBDestroy(sb);
	return str;
}

char* SBBuildWithoutDestroy(StringBuilder *sb)
{
	int i,length=1;
	char* str=NULL;
	for(i=0;i<sb->bufferPointer;i++)
	{
		length+=strlen(sb->bufferedString[i]);
	}
	str = (char*)malloc_s(length*sizeof(char));
	memset(str,0,length*sizeof(char));
	for(i=0;i<sb->bufferPointer;i++)
	{
		strcat(str,sb->bufferedString[i]);
	}
	return str;
}

void SBDestroy(StringBuilder *sb)
{
	free_s(sb);
}

//BKDRHash
/*
Hash HashCode(char* string)
{
	static unsigned long seed = 131;
	unsigned long hash = 0;
	while (*string)  
	{  
		hash = hash * seed + (*string++);  
	}  
	hash &= 0x7FFFFFFF;
	return hash>MAX_STRING_HASH?hash%MAX_STRING_HASH:hash;
}*/

//DJBHash, faster than BKDRHash
Hash HashCode(char* string)
{
	unsigned long hash = 5381;
	while (*string)
	{
		hash += (hash << 5) + (*string++);
	}
	hash &= 0x7FFFFFFF;
	return hash>MAX_STRING_HASH?hash%MAX_STRING_HASH:hash;
}

static const char BINARY_10000000 = 0x80;
static const char BINARY_11000000 = 0xC0;
static const char BINARY_11100000 = 0xE0;
static const char BINARY_11110000 = 0xF0;
static const char BINARY_00011111 = 0x1F;
static const char BINARY_00001111 = 0x0F;
static const char BINARY_00000111 = 0x07;
static const char BINARY_00111111 = 0x3F;

unsigned long* UTF8ToUTF32( char* utf8Text )
{
	unsigned long *buffer;
	unsigned long *utf32Text;
	long sourceLength = strlen(utf8Text);
	char c8;
	unsigned long c32; 
	long i,j;
	buffer = (unsigned long *)malloc_s((sourceLength+1)*sizeof(unsigned long));
	for(i=0,j=0;j<sourceLength;i++,j++)
	{
		c32=0;
		c8=utf8Text[j];
		if(c8>0)
		{
			c32=c8;
		}
		else
		{
			int count;
			if((char)(c8&BINARY_11100000)==BINARY_11100000)
				count=2;
			else if((char)(c8&BINARY_11000000)==BINARY_11000000)
				count=1;
			else if((char)(c8&BINARY_11110000)==BINARY_11110000)
				count=3;
			else
			{
				count=0;
				LoggerWarn("An unknown utf8 character was met when UTF8ToUTF32:%d",(int)c8);
			}
			switch(count)
			{
			case 3:
				c32+=(c8&BINARY_00000111)<<18;
				break;
			case 2:
				c32+=(c8&BINARY_00001111)<<12;
				break;
			case 1:
				c32+=(c8&BINARY_00011111)<<6;
				break;
			default:
				c32=0x25A1;
				break;
			}
			switch(count)
			{
			case 3:
				c8=utf8Text[++j];
				c32+=(c8&BINARY_00111111)<<12;
			case 2:
				c8=utf8Text[++j];
				c32+=(c8&BINARY_00111111)<<6;
			case 1:
				c8=utf8Text[++j];
				c32+=(c8&BINARY_00111111);
				break;
			}
		}
		buffer[i]=c32;
	}
	buffer[i]=0;
	utf32Text=(unsigned long*)malloc_s((i+1)*sizeof(unsigned long));
	memcpy(utf32Text,buffer,(i+1)*sizeof(unsigned long));
	free_s(buffer);
	return utf32Text;
}

wchar_t* UTF8ToANSI( char* utf8Text )
{
	unsigned long length = strlen(utf8Text);
	wchar_t* ansiText = (wchar_t*)malloc_s((length+2)*sizeof(wchar_t));
	memset(ansiText,0,(length+2)*sizeof(wchar_t));
	if(OS_UTF8ToANSI_DO(utf8Text,ansiText,length))
	{
		LoggerWarn("A possible overflow happened when UTF8ToANSI:%s",utf8Text);
	}
	return ansiText;
}

byte LESReadByte(FILE *file)
{
	return (byte)fgetc(file);
}

void LESReadBytes(FILE *file,byte *buffer,size_t count)
{
	fread(buffer,sizeof(byte),count,file);
}

char LESReadChar(FILE *file)
{
	return fgetc(file);
}

float LESReadFloat(FILE *file)
{
	byte buffer[4];
	fread(&buffer,sizeof(byte),4,file);
	return *((float*)(&buffer));
}

double LESReadDouble(FILE *file)
{
	byte buffer[8];
	fread(&buffer,sizeof(byte),8,file);
	return *((double*)(&buffer));
}

short LESReadInt16(FILE *file)
{
	byte buffer[2];
	fread(&buffer,sizeof(byte),2,file);
	return *((short*)(&buffer));
}

long LESReadInt32(FILE *file)
{
	byte buffer[4];
	fread(&buffer,sizeof(byte),4,file);
	return *((long*)(&buffer));
}

long long LESReadInt64(FILE *file)
{
	byte buffer[8];
	fread(&buffer,sizeof(byte),8,file);
	return *((long long*)(&buffer));
}

unsigned short LESReadUInt16(FILE *file)
{
	byte buffer[2];
	fread(&buffer,sizeof(byte),2,file);
	return *((unsigned short*)(&buffer));
}
unsigned long  LESReadUInt32(FILE *file)
{
	byte buffer[4];
	fread(&buffer,sizeof(byte),4,file);
	return *((unsigned long*)(&buffer));
}
