#include "memory.h"

static long pointerUsed = 0;

void* malloc_s( size_t size )
{
	void* p = malloc(size);
	if(p==NULL)
	{
		//TODO:OOM!
	}
	pointerUsed++;
	return p;
}

void* realloc_s( void* p,size_t size )
{
	void* _p = realloc(p,size);
	if(_p==NULL)
	{
		//TODO:OOM!
	}
	return p;
}

void free_s( void* p )
{
	pointerUsed--;
	free(p);
}

long GetRemainedPointerCount()
{
	return pointerUsed;
}