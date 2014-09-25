#include <stdlib.h>

void* malloc_s(size_t size);
void* realloc_s(void* p,size_t size);
void free_s(void* p);
long GetRemainedPointerCount();