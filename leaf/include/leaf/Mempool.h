/*
 ==============================================================================

 Mempool.h
 C++ class replacing the C tMempool type.

 ==============================================================================
*/

#ifndef LEAF_MEMPOOL_CLASS_H_INCLUDED
#define LEAF_MEMPOOL_CLASS_H_INCLUDED

#include <stddef.h>

//==============================================================================
// Types shared between C and C++ — defined here so Mempool.h is self-contained.
// leaf-mempool.h (C mode) duplicates these; no conflict since C code never
// includes this file.
//==============================================================================

#define MPOOL_ALIGN_SIZE (8)

typedef enum LEAFErrorType {
    LEAFMempoolOverrun = 0,
    LEAFMempoolFragmentation,
    LEAFInvalidFree,
    LEAFErrorNil
} LEAFErrorType;

// node of free list
typedef struct mpool_node_t {
    char *pool;                // memory pool field
    struct mpool_node_t *next; // next node pointer
    struct mpool_node_t *prev; // prev node pointer
    size_t size;
} mpool_node_t;

// Aligned size of a node header
#define MPOOL_HEADER_SIZE (((sizeof(mpool_node_t)) + (MPOOL_ALIGN_SIZE - 1)) & ~(size_t)(MPOOL_ALIGN_SIZE - 1))

//==============================================================================

// Forward-declare LEAF — only the pointer is needed in the declarations below.
namespace leaf { struct LEAF; }

//==============================================================================

namespace leaf {

/*!
 @class Mempool
 @brief Memory pool for the allocation of LEAF objects.

 Replaces the C tMempool type.  The LEAF struct embeds one Mempool by value
 (_internal_mempool); all user-allocated mempools are pool-managed pointers
 created via tMempool_init / tMempool_initToPool.
*/
class Mempool {
public:
    /// Default constructor — fields zero-initialised; call initAsRoot() or
    /// the pool-parent constructor before use.
    Mempool() = default;

    /// Construct a child pool allocated from @p parent.
    Mempool(char *memory, size_t size, Mempool *parent);

    ~Mempool() = default;

    Mempool *parentPool() const { return mempool_; }

    /// Initialise as the root pool embedded in a LEAF instance.
    /// @p errorCb  Error handler (typically LEAF_defaultErrorCallback).
    void initAsRoot(char *memory, size_t size,
                    void (*errorCb)(Mempool *const, LEAFErrorType));

    void   create      (char *memory, size_t size);
    char  *alloc       (size_t size);
    char  *allocZeroed (size_t size);
    void   free        (char *ptr);

    size_t getSize() const { return msize_; }
    size_t getUsed() const { return usize_; }
    char  *getPool() const { return mpool_; }

    void setErrorCallback  (void (*cb)(Mempool *const, LEAFErrorType)) { errorCallback_ = cb; }
    void setClearOnAllocation(int v) { clearOnAllocation_ = v; }

private:
    Mempool      *mempool_           = nullptr;
    char         *mpool_             = nullptr;
    size_t        usize_             = 0;
    size_t        msize_             = 0;
    mpool_node_t *head_              = nullptr;
    int           clearOnAllocation_ = 0;
    unsigned int  allocCount_        = 0;
    unsigned int  freeCount_         = 0;
    int           errorState_[LEAFErrorNil] = {};
    void        (*errorCallback_)(Mempool *const, LEAFErrorType) = nullptr;

    void internalErrorCallback(LEAFErrorType which);
};

} // namespace leaf

/// Backward-compatible C++ alias — existing code using tMempool continues to work.
typedef leaf::Mempool tMempool;

//==============================================================================
// C shims — keep the original names so all C and C++ callers are unchanged.
// Use leaf::LEAF* directly to avoid adding a global 'using' alias here.
//==============================================================================

extern "C" {

void tMempool_init      (tMempool **const mp, char *memory, size_t size, leaf::LEAF *const leaf);
void tMempool_free      (tMempool **const mp);
void tMempool_initToPool(tMempool **const mp, char *memory, size_t size, tMempool **const mem);

void   mpool_create   (char *memory, size_t size, tMempool *pool);
char  *mpool_alloc    (size_t size, tMempool *pool);
char  *mpool_calloc   (size_t asize, tMempool *pool);
void   mpool_free     (char *ptr, tMempool *pool);
size_t mpool_get_size (tMempool *pool);
size_t mpool_get_used (tMempool *pool);

void   leaf_pool_init    (leaf::LEAF *const leaf, char *memory, size_t size);
char  *leaf_alloc        (leaf::LEAF *const leaf, size_t size);
char  *leaf_calloc       (leaf::LEAF *const leaf, size_t size);
void   leaf_free         (leaf::LEAF *const leaf, char *ptr);
size_t leaf_pool_get_size(leaf::LEAF *const leaf);
size_t leaf_pool_get_used(leaf::LEAF *const leaf);
char  *leaf_pool_get_pool(leaf::LEAF *const leaf);

} // extern "C"

#endif // LEAF_MEMPOOL_CLASS_H_INCLUDED
