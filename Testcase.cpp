#include <cstdio>
#include <cstdlib>
#include <time.h>
#include "Mempool.h"

#define MEMPOOL_SIZE 100*1024*1024
#define ARRAY_SZIE 200

int main()
{
	int result = Mempool::InitPool(MEMPOOL_SIZE);
	if ( result != 0 )
	{
		printf("Mempool init error\n");
		return 0;	
	}
	Mempool::Dump();
	srand(time(NULL));
	unsigned char* array[ARRAY_SZIE];
	for ( int i = 0; i < ARRAY_SZIE; ++i )
	{
		size_t size = rand() % 6000;
		array[i] = (unsigned char*) Mempool::Allocate(size);
	}
	Mempool::Dump();
	for ( int i = 0; i < ARRAY_SZIE; ++i )
	{
		unsigned char* p = array[i];
		Mempool::Deallocate((void*)p);
	}
	Mempool::Dump();
	Mempool::DestroyPool();
	return 0;
}
