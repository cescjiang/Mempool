#define _MIN_ALLOC_SIZE 8
#define _MAX_ALLOC_SIZE 4096
#define _LINK_LIST_SIZE (_MAX_ALLOC_SIZE/_MIN_ALLOC_SIZE)
#define _ALIGN_SIZE 8

class Mempool
{
    struct _chunk_obj
    {
        unsigned char* __next;
        unsigned char chunk[];
    }

    struct _pool_status
    {
        _pool_status() : __large_free_list(NULL),
                         __start(NULL),
                         __end(NULL)
        {
            for ( int i = 0; i < _LINK_LIST_SIZE; +i ) {
                __free_link_list[i] = NULL;
            }
        }
        unsigned char* __free_link_list[_LINK_LIST_SIZE];
        unsigned char* __large_free_list;
        unsigned char* __start;
        unsigned char* __end;
    };
public:
    static void init_mempool(size_t size);
    static void* allocate(size_t size);
    static void dellocate(void* pointer);
private:
    Mempool();
    ~Mempool();
    static unsigned char* _start_free;
    static unsigned char* _end_free;
    static unsigned char* _start;
};

