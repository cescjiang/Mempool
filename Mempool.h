#ifndef __MEMPOOL
#define __MEMPOOL

#include <cstddef>

#define _MIN_ALLOC_SIZE 8
#define _MAX_ALLOC_SIZE 4096
#define _LINK_LIST_SIZE (_MAX_ALLOC_SIZE/_MIN_ALLOC_SIZE)
#define _REALLOCATE_CHUNK_CNT 20

class Mempool
{
    struct _chunk_obj
    {
        struct _chunk_obj* __next;
        unsigned char chunk[];
    };
public:
    static int init_pool(size_t size);
    static void destroy_pool();
    static void* allocate(size_t size);
    static void deallocate(void* pointer);
    static void dump();
private:
    static size_t _round_up(size_t size);
    static int _freelist_index(size_t size);
    static int _reallocate(size_t size, int default_cnt = 20);
private:
    Mempool();
    ~Mempool();
    static struct _chunk_obj* _free_link_list[_LINK_LIST_SIZE];
    static struct _chunk_obj* _large_free_list;
    static unsigned char* _start;
    static unsigned char* _start_free;
    static unsigned char* _end_free;
};

#endif
