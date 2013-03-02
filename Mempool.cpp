#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <cassert>
#include "Mempool.h"

struct Mempool::_chunk_obj* Mempool::_free_link_list[];
struct Mempool::_chunk_obj* Mempool::_large_free_list = NULL;
unsigned char* Mempool::_start = NULL;
unsigned char* Mempool::_start_free = NULL;
unsigned char* Mempool::_end_free = NULL;

int Mempool::init_pool(size_t size)
{
    for ( int i = 0; i < _LINK_LIST_SIZE; ++i )
    {
        _free_link_list[i] = NULL;
    }
    _large_free_list = NULL;

    unsigned char* pointer;
    pointer = (unsigned char*) malloc(size);
    if ( pointer == NULL )
    {
        printf("mempool init fail\n");
        return -1;
    }
    memset(pointer, 0, size);
    _start = pointer;
    _start_free = pointer;
    _end_free = pointer + size;
    return 0;
}

void Mempool::destroy_pool()
{
    if ( _start_free != NULL )
    {
        free(_start);
    }
}

size_t Mempool::_round_up(size_t size)
{
    size_t res = size + (_MIN_ALLOC_SIZE - 1) & ~(_MIN_ALLOC_SIZE - 1);
    return res == 0 ? _MIN_ALLOC_SIZE : res;
}

int Mempool::_freelist_index(size_t size)
{
    size_t align_size = _round_up(size);
    return align_size / _MIN_ALLOC_SIZE - 1;
}

int Mempool::_reallocate(size_t size, int default_cnt)
{
    size_t align_size = _round_up(size);
    size_t real_size = align_size + sizeof(size_t);
    int cnt = (_end_free - _start_free) / real_size;
    cnt = cnt > default_cnt ? default_cnt : cnt;
    // 空间不够了
    if ( cnt <= 0 )
    {
        return -1;
    }
    // 分配若干个新的chunk
    unsigned char* start = _start_free;
    for ( int i = 0; i < cnt; ++i )
    {
        struct _chunk_obj* chunk = (struct _chunk_obj*)(start + sizeof(size_t));
        start += real_size;
        chunk->__next = (struct _chunk_obj*)(start + sizeof(size_t));
        *((size_t*)chunk - 1) = align_size;
    }
    // 将最后一个chunk->__next置为NULL
    struct _chunk_obj* last_chunk = (struct _chunk_obj*)(start - real_size + sizeof(size_t));
    last_chunk->__next = NULL;
    // 第一块chunk为_free__link_list
    int index = _freelist_index(size);
    assert(index >= 0 && index < _LINK_LIST_SIZE);
    _free_link_list[index] = (struct _chunk_obj*)(_start_free + sizeof(size_t));
    // 更新_start_free
    _start_free = start;
    return 0;
}

void* Mempool::allocate(size_t size)
{
    if ( size <= 0 )
    {
        return NULL;
    }
    // 小块内存在自由链表上找
    if ( size <= _MAX_ALLOC_SIZE )
    {
        int index = _freelist_index(size);
        assert(index >= 0 && index < _LINK_LIST_SIZE);
        if ( _free_link_list[index] == NULL )
        {
            // 自由链表为空，则重新分配一些chunk加在链表上
            // 默认分配的个数是20个
            if ( _reallocate(size) != 0 )
            {
                return NULL;
            }
        }
        struct _chunk_obj* ret = _free_link_list[index];
        _free_link_list[index] = ret->__next;
        return (void*)(ret);
    }
    // 分配大块内存 
    size_t align_size = _round_up(size);
    struct _chunk_obj* chunk = _large_free_list;
    // 先在大块内存链表上查找一块合适的
    while ( chunk != NULL )
    {
        if ( *((size_t*)chunk - 1) == align_size )
        {
            _large_free_list = chunk->__next;
            return (void*) chunk;
        }
        chunk = chunk->__next;
    }
    // 找不到就重新分配一块
    if ( _end_free - _start_free >= static_cast<int>(align_size + sizeof(size_t)) )
    {
        struct _chunk_obj* chunk = (struct _chunk_obj*)(_start_free + sizeof(size_t));
        *((size_t*)chunk - 1) = align_size;
        _start_free += align_size + sizeof(size_t);
        return (void*) chunk;
    }
    return NULL;
}

void Mempool::deallocate(void* pointer)
{
    if ( pointer == NULL )
    {
        return;
    }
    struct _chunk_obj* chunk = (struct _chunk_obj*) pointer;
    size_t align_size = *((size_t*)pointer - 1);
    if ( align_size <= _MAX_ALLOC_SIZE )
    {
        int index = align_size / _MIN_ALLOC_SIZE - 1;
        assert(index >= 0 && index < _LINK_LIST_SIZE);
        chunk->__next = _free_link_list[index];
        _free_link_list[index] = chunk;
    }
    else
    {
        chunk->__next = _large_free_list;
        _large_free_list = chunk;
    }
}

void Mempool::dump()
{
    printf("-----------dump-----------\n");
    printf("freelist_link:\n");
    for ( int i = 0; i < _LINK_LIST_SIZE; ++i )
    {
        int list_length = 0;
        struct _chunk_obj* chunk = _free_link_list[i];
        while ( chunk != NULL )
        {
            ++list_length;
            chunk = chunk->__next;
        }
        if ( list_length != 0 )
        {
            printf("freelist_link[%d] size=%d\n", (i + 1) * _MIN_ALLOC_SIZE, list_length);
        }
    }
    printf("large_free_list:\n");
    struct _chunk_obj* chunk = _large_free_list;
    while ( chunk != NULL )
    {
        size_t align_size = *((size_t*)chunk - 1);
        printf("chunk size = %lu -->\n", align_size);
        chunk = chunk->__next;
    }
    printf("_start_free = %p, _end_free = %p, free_size = %ld\n", _start_free, _end_free, _end_free - _start_free);
}


