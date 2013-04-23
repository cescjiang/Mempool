#ifndef __MEMPOOL
#define __MEMPOOL

#include <cstddef>

#define _MIN_ALLOC_SIZE 8
#define _MAX_ALLOC_SIZE 4096
#define _LINK_LIST_SIZE (_MAX_ALLOC_SIZE/_MIN_ALLOC_SIZE)
#define _REALLOCATE_CHUNK_CNT 20

class Mempool
{
    struct _ChunkObj
    {
        struct _ChunkObj* __next;
        unsigned char chunk[];
    };
public:
    static int InitPool(size_t size);
    static void DestroyPool();
    static void* Allocate(size_t size);
    static void Deallocate(void* pointer);
    static void Dump();
private:
    static size_t _RoundUp(size_t size);
    static int _FreelistIndex(size_t size);
    static int _Reallocate(size_t size, int default_cnt = 20);
private:
    Mempool();
    ~Mempool();
    static struct _ChunkObj* _free_link_list[_LINK_LIST_SIZE];
    static struct _ChunkObj* _large_free_list;
    static unsigned char* _start;
    static unsigned char* _start_free;
    static unsigned char* _end_free;
};

#endif
