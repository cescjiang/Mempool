#define _MIN_ALLOC_SIZE 8
#define _MAX_ALLOC_SIZE 4096
#define _LINK_LIST_SIZE (_MAX_ALLOC_SIZE/_MIN_ALLOC_SIZE)
#define _ALIGN_SIZE 8
#define _ALIGN_SIZE_LARGE 4096
#define _REALLOCATE_CHUNK_CNT 20

class Mempool
{
    struct _chunk_obj
    {
        struct _chunk_obj* __next;
        unsigned char __chunk[];
    };
public:
    static void init_mempool(size_t size);
    static void* allocate(size_t size);
    static void dellocate(void* pointer);
private:
    static uint64_t _round_up(uint64_t size);
    static struct _chunk_obj* _reallocate(size_t size);
    static struct _chunk_obj* _find_large_chunk(size_t size);
private:
    Mempool();
    ~Mempool();
    struct _chunk_obj* _free_link_list[LINK_LIST_SIZE];
    struct _chunk_obj* _large_free_list;
    static unsigned char* _start_free;
    static unsigned char* _end_free;
};

unsigned char* Mempool::_start_free = NULL;
unsigned char* Mempool::_end_free = NULL;

