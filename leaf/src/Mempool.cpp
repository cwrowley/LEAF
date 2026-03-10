/*
 ==============================================================================

 Mempool.cpp
 C++ class implementation for leaf::Mempool, plus C shims.

 ==============================================================================
*/

#include <Mempool.h>
#include <leaf.h>

#include <new>
#include <stdlib.h>
#include <string.h>

#if LEAF_DEBUG
#include "../../TestPlugin/JuceLibraryCode/JuceHeader.h"
#endif

//==============================================================================
// Private helpers (file-static)
//==============================================================================

static inline size_t mpool_align(size_t size) {
    return (size + (MPOOL_ALIGN_SIZE - 1)) & ~(MPOOL_ALIGN_SIZE - 1);
}

static inline mpool_node_t *create_node(char *block_location,
                                         mpool_node_t *next,
                                         mpool_node_t *prev,
                                         size_t size) {
    mpool_node_t *node = (mpool_node_t *)block_location;
    node->pool = block_location + MPOOL_HEADER_SIZE;
    node->next = next;
    node->prev = prev;
    node->size = size;
    return node;
}

static inline void delink_node(mpool_node_t *node) {
    if (node->next != NULL) node->next->prev = node->prev;
    if (node->prev != NULL) node->prev->next = node->next;
    node->next = NULL;
    node->prev = NULL;
}

//==============================================================================
// Class method definitions
//==============================================================================

static void defaultErrorCallback(leaf::Mempool *const, LEAFErrorType) {}

namespace leaf {

Mempool::Mempool(char *memory, size_t size) {
    mempool_           = nullptr;
    clearOnAllocation_ = 0;
    allocCount_        = 0;
    freeCount_         = 0;
    for (int i = 0; i < LEAFErrorNil; ++i)
        errorState_[i] = 0;
    errorCallback_     = defaultErrorCallback;
    create(memory, size);
}

Mempool::Mempool(char *memory, size_t size, Mempool &parent) {
    mempool_           = &parent;
    clearOnAllocation_ = parent.clearOnAllocation_;
    errorCallback_     = parent.errorCallback_;
    allocCount_        = 0;
    freeCount_         = 0;
    for (int i = 0; i < LEAFErrorNil; ++i)
        errorState_[i] = 0;
    create(memory, size);
}

void Mempool::create(char *memory, size_t size) {
    mpool_ = memory;
    usize_ = 0;
    if (size < MPOOL_HEADER_SIZE)
        size = MPOOL_HEADER_SIZE;
    msize_ = size;
    head_  = create_node(mpool_, NULL, NULL, msize_ - MPOOL_HEADER_SIZE);
}

void Mempool::internalErrorCallback(LEAFErrorType which) {
    errorState_[which] = 1;
    errorCallback_(this, which);
}

char *Mempool::alloc(size_t asize) {
    allocCount_++;
#if LEAF_DEBUG
    DBG("alloc " + String(asize));
#endif
#if LEAF_USE_DYNAMIC_ALLOCATION
    char *temp = (char *)malloc(asize);
    if (temp == NULL) {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    if (clearOnAllocation_ > 0)
        memset(temp, 0, asize);
    return temp;
#else
    if (head_ == NULL) {
        if ((msize_ - usize_) > asize)
            internalErrorCallback(LEAFMempoolFragmentation);
        else
            internalErrorCallback(LEAFMempoolOverrun);
        return NULL;
    }

    size_t size_to_alloc    = mpool_align(asize);
    mpool_node_t *node_to_alloc = head_;

    while (node_to_alloc->size < size_to_alloc) {
        node_to_alloc = node_to_alloc->next;
        if (node_to_alloc == NULL) {
            if ((msize_ - usize_) > asize)
                internalErrorCallback(LEAFMempoolFragmentation);
            else
                internalErrorCallback(LEAFMempoolOverrun);
            return NULL;
        }
    }

    mpool_node_t *new_node;
    size_t leftover      = node_to_alloc->size - size_to_alloc;
    node_to_alloc->size  = size_to_alloc;
    if (leftover > MPOOL_HEADER_SIZE) {
        long offset = (char *)node_to_alloc - mpool_;
        offset += MPOOL_HEADER_SIZE + node_to_alloc->size;
        new_node = create_node(&mpool_[offset],
                               node_to_alloc->next,
                               node_to_alloc->prev,
                               leftover - MPOOL_HEADER_SIZE);
    } else {
        node_to_alloc->size += leftover;
        new_node = node_to_alloc->next;
    }

    if (head_ == node_to_alloc)
        head_ = new_node;

    delink_node(node_to_alloc);

    usize_ += MPOOL_HEADER_SIZE + node_to_alloc->size;

    if (clearOnAllocation_ > 0) {
        char *new_pool = node_to_alloc->pool;
        for (int i = 0; i < (int)node_to_alloc->size; i++)
            new_pool[i] = 0;
    }

    return node_to_alloc->pool;
#endif
}

char *Mempool::allocZeroed(size_t asize) {
    allocCount_++;
#if LEAF_DEBUG
    DBG("calloc " + String(asize));
#endif
#if LEAF_USE_DYNAMIC_ALLOCATION
    char *ret = (char *)malloc(asize);
    if (ret == NULL) {
        fprintf(stderr, "Out of memory.\n");
        exit(1);
    }
    memset(ret, 0, asize);
    return ret;
#else
    if (head_ == NULL) {
        if ((msize_ - usize_) > asize)
            internalErrorCallback(LEAFMempoolFragmentation);
        else
            internalErrorCallback(LEAFMempoolOverrun);
        return NULL;
    }

    size_t size_to_alloc    = mpool_align(asize);
    mpool_node_t *node_to_alloc = head_;

    while (node_to_alloc->size < size_to_alloc) {
        node_to_alloc = node_to_alloc->next;
        if (node_to_alloc == NULL) {
            if ((msize_ - usize_) > asize)
                internalErrorCallback(LEAFMempoolFragmentation);
            else
                internalErrorCallback(LEAFMempoolOverrun);
            return NULL;
        }
    }

    mpool_node_t *new_node;
    size_t leftover     = node_to_alloc->size - size_to_alloc;
    node_to_alloc->size = size_to_alloc;
    if (leftover > MPOOL_HEADER_SIZE) {
        long offset = (char *)node_to_alloc - mpool_;
        offset += MPOOL_HEADER_SIZE + node_to_alloc->size;
        new_node = create_node(&mpool_[offset],
                               node_to_alloc->next,
                               node_to_alloc->prev,
                               leftover - MPOOL_HEADER_SIZE);
    } else {
        node_to_alloc->size += leftover;
        new_node = node_to_alloc->next;
    }

    if (head_ == node_to_alloc)
        head_ = new_node;

    delink_node(node_to_alloc);

    usize_ += MPOOL_HEADER_SIZE + node_to_alloc->size;

    for (int i = 0; i < (int)node_to_alloc->size; i++)
        node_to_alloc->pool[i] = 0;

    return node_to_alloc->pool;
#endif
}

void Mempool::free(char *ptr) {
    freeCount_++;
#if LEAF_DEBUG
    DBG("free");
#endif
#if LEAF_USE_DYNAMIC_ALLOCATION
    ::free(ptr);
#else
    mpool_node_t *freed_node = (mpool_node_t *)(ptr - MPOOL_HEADER_SIZE);

    usize_ -= MPOOL_HEADER_SIZE + freed_node->size;

    mpool_node_t *other_node = head_;
    mpool_node_t *next_node;
    while (other_node != NULL) {
        if ((long)other_node < (long)mpool_ ||
            (long)other_node >= ((long)mpool_ + msize_)) {
            internalErrorCallback(LEAFInvalidFree);
            return;
        }
        next_node = other_node->next;

        if (((long)freed_node) + (MPOOL_HEADER_SIZE + freed_node->size) == (long)other_node) {
            freed_node->size += MPOOL_HEADER_SIZE + other_node->size;
            if (other_node == head_)
                head_ = head_->next;
            delink_node(other_node);
        } else if (((long)other_node) + (MPOOL_HEADER_SIZE + other_node->size) == (long)freed_node) {
            other_node->size += MPOOL_HEADER_SIZE + freed_node->size;
            if (other_node != head_) {
                delink_node(other_node);
                other_node->next = head_;
                freed_node = other_node;
            } else {
                head_ = head_->next;
                freed_node = other_node;
            }
        }

        other_node = next_node;
    }

    freed_node->next = head_;
    if (head_ != NULL)
        head_->prev = freed_node;
    head_ = freed_node;
#endif
}

} // namespace leaf

//==============================================================================
// C shims
//==============================================================================

void mpool_create(char *memory, size_t size, tMempool *pool) {
    pool->create(memory, size);
}

char *mpool_alloc(size_t size, tMempool *pool) {
    return pool->alloc(size);
}

char *mpool_calloc(size_t asize, tMempool *pool) {
    return pool->allocZeroed(asize);
}

void mpool_free(char *ptr, tMempool *pool) {
    if (pool != NULL)
        pool->free(ptr);
}

size_t mpool_get_size(tMempool *pool) { return pool->getSize(); }
size_t mpool_get_used(tMempool *pool) { return pool->getUsed(); }

void tMempool_init(tMempool **const mp, char *memory, size_t size, leaf::LEAF *const leaf) {
    leaf::Mempool *p = leaf->mempool();
    tMempool_initToPool(mp, memory, size, &p);
}

void tMempool_free(tMempool **const mp) {
    leaf::Mempool *m = *mp;
    mpool_free((char *)m, m->parentPool());
}

void tMempool_initToPool(tMempool **const mp, char *memory, size_t size, tMempool **const mem) {
    leaf::Mempool *mm = *mem;
    leaf::Mempool *m  = *mp = new (mm->alloc(sizeof(leaf::Mempool))) leaf::Mempool(memory, size, *mm);
}

void leaf_pool_init(leaf::LEAF *const leaf, char *memory, size_t size) {
    // Friend access: constructs the root mempool in-place inside the LEAF instance.
    new (&leaf->_internal_mempool_) leaf::Mempool(memory, size);
    leaf->mempool_ = &leaf->_internal_mempool_;
}

char  *leaf_alloc        (leaf::LEAF *const leaf, size_t size) { return leaf->mempool()->alloc(size); }
char  *leaf_calloc       (leaf::LEAF *const leaf, size_t size) { return leaf->mempool()->allocZeroed(size); }
void   leaf_free         (leaf::LEAF *const leaf, char *ptr)   { leaf->mempool()->free(ptr); }
size_t leaf_pool_get_size(leaf::LEAF *const leaf)              { return leaf->mempool()->getSize(); }
size_t leaf_pool_get_used(leaf::LEAF *const leaf)              { return leaf->mempool()->getUsed(); }
char  *leaf_pool_get_pool(leaf::LEAF *const leaf)              { return leaf->mempool()->getPool(); }
