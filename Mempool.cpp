#include "Mempool.h"

void Mempool::init_mempool(size_t size)
{
    for ( int i = 0; i < LINK_LIST_SIZE; ++i ) {
        _free_link_list[i] = NULL;
    }
    _large_free_list = NULL;

    unsigned char* pointer;
    uint64_t align_size = _round_up(size);
    try {
        pointer = (unsigned char*) malloc(align_size);
    }
    catch ( const std::exception& e) {
        printf("%s\n", e.what());
        return -1;
    }
    memset(pointer, 0, align_size);
    _start_free = pointer;
    _end_free = pointer + align_size;
}


uint64_t Mempool::_round_up(uint64_t size, int align)
{
    assert(align > 0);
    uint64_t res = size + (align - 1) & ~(align - 1);
    return res == 0 ? align : res;
}

int Mempool::_freelist_index(uint64_t size)
{
    uint64_t align_size = _round_up(size, _ALIGN_SIZE);
    return align_size / _ALIGN_SIZE - 1;
}

struct _chunk_obj* Mempool::_reallocate(size_t size)
{
    uint64_t align_size = _round_up(size, _ALIGN_SIZE);
    uint64_t real_size = align_size + sizeof(uint64_t);
    int cnt = (_end_free - _start_free) / real_size;
    cnt = cnt > _REALLOCATE_CHUNK_CNT ? _REALLOCATE_CHUNK_CNT : cnt;
    unsigned char* start = _start_free;
    
    struct _chunk_obj* chunk = NULL;
    for ( int i = 0; i < cnt; ++i )
    {
        chunk = reinterpret_cast<struct _chunk_obj*>
                    (start + sizeof(uint64_t));
        start += real_size;
        chunk->__next =
            reinterpret_cast<struct _chunk_obj*>
                (start + sizeof(uint64_t));
    }
    chunk->__next = NULL;

    struct _chunk_obj* first =
        reinterpret_cast<struct _chunk_obj*>
            (_start_free + sizeof(uint64_t));
    _start_free += (cnt * real_size);
    return cnt == 0 ? NULL : first;
}

struct _chunk_obj* Mempool::_find_large_chunk(size_t size)
{
    uint64_t align_size = _round_up(size, _ALIGN_SIZE_LARGE);
    struct _chunk_obj* chunk = _large_free_list;
    while ( chunk != NULL )
    {
        uint64_t chunk_size =
            *(reinterpret_cast<uint64_t*>(chunk) - 1);
        if ( chunk_size == align_size ) {
            return chunk;
        }
        chunk = chunk->__next;
    }
    return NULL;
}

void* Mempool::allocate(size_t size)
{
    if ( size <= 0 ) {
        return NULL;
    }

    if ( size <= _MAX_ALLOC_SIZE )
    {
        int index = _freelist_index(size);
        if ( _free_link_list[index] == NULL )
        {
            struct _chunk_obj* new_chunk = _reallocate(size);
            if ( new_chunk == NULL ) {
                return NULL;
            }
            _freelist_link_list[index] = new_chunk;
        }

        struct _chunk_obj* ret = _free_link_list[index];
        _free_link_list[index] = ret->next;
        return reinterpret_cast<void*>(ret);
    }
    else
    {
        struct _chunk_obj* ret = _find_large_chunk(size);
        if ( ret != NULL ) {
            _large_free_list = ret->__next;
            return reinterpret_cast<void*>(ret);
        }

        uint64_t remainder_size = _end_free - _start_free;
        uint64_t align_size = _round_up(size, _ALIGN_SIZE_LARGE);
        if ( remainder_size <
                align_size + sizeof(uint64_t) ) {
            return NULL;
        }
        *reinterpret_cast<uint64_t*>(_start_free) = align_size;
        ret = reinterpret_cast<struct _chunk_obj*>
                (_start_free + sizeof(uint64_t));
        _start_free += (align_size + sizeof(uint64_t));
        return reinterpret_cast<void*>(ret);
    }
}

void Mempool::dellocate(void* pointer)
{
    if ( pointer == NULL ) {
        return;
    }
    uint64_t size = *(reinterpret_cast<uint64_t*>(pointer) - 1);
    assert(size >= _ALIGN_SIZE);
    if ( size <= _MAX_ALLOC_SIZE )
    {
        int index = size / _ALIGN_SIZE - 1;
        struct _chunk_obj* chunk =
            reinterpret_cast<struct _chunk_obj*>(pointer);
        chunk->__next = _free_link_list[index];
        _free_link_list[index] = chunk;
    }
    else
    {
        struct _chunk_obj* chunk =
            reinterpret_cast<struct _chunk_obj*>(pointer);
        chunk->__next = _large_free_list;
        _large_free_list = chunk;
    }
}


