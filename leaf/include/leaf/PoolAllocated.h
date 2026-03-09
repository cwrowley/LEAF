/*
 ==============================================================================

 PoolAllocated.h
 CRTP base class for LEAF objects that can be allocated from a mempool.

 Usage
 -----
 1. Inherit your class from PoolAllocated<Derived>.
 2. Define a single constructor:  Foo(LeafInit ctx, <extra args>...)
    and initialise the base:       : PoolAllocated<Foo>(ctx), ...
 3. Stack allocation:   Foo obj(*leaf, <extra args>...);
    Pool allocation:    Foo *obj = Foo::create(*leaf, <extra args>...);
    Explicit pool:      Foo *obj = Foo::create(*leaf, myPool, <extra args>...);
    Destruction:        Foo::destroy(obj);   // frees from the correct pool

 ==============================================================================
*/

#ifndef LEAF_POOL_ALLOCATED_H_INCLUDED
#define LEAF_POOL_ALLOCATED_H_INCLUDED

#include <leaf-global.h>
#include <Mempool.h>
#include <new>
#include <utility>

namespace leaf {

/*!
 @struct LeafInit
 @brief  Lightweight context passed as the first constructor argument of every
         PoolAllocated subclass.  Implicitly constructible from a LEAF reference
         so that stack allocation reads  Foo obj(*leaf, extra...);
*/
struct LeafInit {
    LEAF    &leaf;
    Mempool &mempool;

    /// Implicit conversion from LEAF& — uses the default mempool.
    LeafInit(LEAF &l)             : leaf(l), mempool(*l.mempool) {}
    /// Explicit construction with a specific mempool.
    LeafInit(LEAF &l, Mempool &m) : leaf(l), mempool(m) {}
};

/*!
 @class PoolAllocated
 @brief CRTP base providing pool-aware factory methods and mempool storage.

 Subclasses inherit create() and destroy() for free; mempool_ is stored here
 and exposed via the public mempool() accessor.
*/
template<typename Derived>
class PoolAllocated {
public:
    Mempool *mempool() const { return mempool_; }

    /// Allocate from the LEAF instance's default mempool.
    template<typename... Args>
    static Derived *create(LEAF &leaf, Args &&...args) {
        Mempool &m  = *leaf.mempool;
        void    *mem = m.alloc(sizeof(Derived));
        return new (mem) Derived(LeafInit{leaf, m}, std::forward<Args>(args)...);
    }

    /// Allocate from an explicit mempool.
    template<typename... Args>
    static Derived *create(LEAF &leaf, Mempool &m, Args &&...args) {
        void *mem = m.alloc(sizeof(Derived));
        return new (mem) Derived(LeafInit{leaf, m}, std::forward<Args>(args)...);
    }

    /// Destroy and free back to the pool the object was allocated from.
    static void destroy(Derived *&obj) {
        Mempool *m = obj->mempool_;
        obj->~Derived();
        m->free((char *)obj);
        obj = nullptr;
    }

protected:
    explicit PoolAllocated(LeafInit ctx) : mempool_(&ctx.mempool) {}
    Mempool *mempool_;

    char *palloc(size_t size) { return mempool_->alloc(size); }
    void  pfree(char *ptr)    { mempool_->free(ptr); }
};

} // namespace leaf

#endif // LEAF_POOL_ALLOCATED_H_INCLUDED
