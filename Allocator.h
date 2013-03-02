#include <cstddef>
#include <new>
#include "Mempool.h"

template<typename T>
class Allocator
{
public:
    typedef T         value_type;
    typedef T*        pointer;
    typedef const T*  const_pointer;
    typedef T&        reference;
    typedef const T&  const_reference;
    typedef size_t    size_type;
    typedef ptrdiff_t difference_type;

    template<typename _U>
    struct rebind {
        typedef Allocator<_U> other;
    };
    // 默认构造函数
    Allocator() { }
    // 析构函数
    ~Allocator() { }
    // 拷贝构造函数
    Allocator(const Allocator&) { }
    // 泛化的拷贝构造函数
    template<typename _U>
    Allocator(const Allocator<_U>&) { }
    // 返回对象地址
    pointer address(reference x) const {
        return (pointer) &x;
    }
    // 返回const对象地址
    const_pointer address(const_reference x) const {
        return (const_pointer) &x;
    }
    // 可成功配置的最大数
    size_type max_size() {
        return size_type(-1)/sizeof(T);
    }
    // 配置
    pointer allocate(size_type n, const void* = 0) {
        if ( n <= 0 || n > max_size() ) {
            return NULL;
        }
        pointer p;
        p = reinterpret_cast<pointer>(
            Mempool::Allocate(sizeof(T) * n));
        return p;
    }
    // 归还
    void deallocate(pointer p, size_type) {
        Mempool::Deallocate(p);
    }
    // 对元素进行拷贝构造
    void construct(pointer p, const T& x) {
        new (reinterpret_cast<void*>(p)) T(x);
    }
    // 对元素进行默认构造
    void construct(pointer p) {
        new (reinterpret_cast<void*>(p)) T();
    }
    // 元素的析构函数
    void destroy(pointer p) {
        p->~T();
    }
};

