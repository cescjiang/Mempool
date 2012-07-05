#include "Mempool.h"

void Mempool::init_mempool(size_t size) {
    try
    {
        _start = (unsigned char*) malloc(size);
    }
    catch ( const std::exception& e) {
        assert(false);
        return -1;
    }
}

