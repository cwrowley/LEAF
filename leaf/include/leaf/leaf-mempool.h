/*==============================================================================

 In short, mpool is distributed under so called "BSD license",

 Copyright (c) 2009-2010 Tatsuhiko Kubo <cubicdaiya@gmail.com>
 All rights reserved.

 Redistribution and use in source and binary forms, with or without modification,
 are permitted provided that the following conditions are met:

 * Redistributions of source code must retain the above copyright notice,
 this list of conditions and the following disclaimer.

 * Redistributions in binary form must reproduce the above copyright notice,
 this list of conditions and the following disclaimer in the documentation
 and/or other materials provided with the distribution.

 * Neither the name of the authors nor the names of its contributors
 may be used to endorse or promote products derived from this software
 without specific prior written permission.

 THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED
 TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR
 PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF
 LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING
 NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.

 written by C99 style
 ==============================================================================*/

#ifndef LEAF_MPOOL_H_INCLUDED
#define LEAF_MPOOL_H_INCLUDED

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>

//==============================================================================

#define MPOOL_ALIGN_SIZE (8)

// Forward declaration of LEAF with C/C++ compatibility.
// In C++, LEAF lives in namespace leaf; a global alias is provided for
// backward compatibility.  In C, a plain typedef is sufficient.
#ifdef __cplusplus
namespace leaf { struct LEAF; }
using LEAF = leaf::LEAF;
#else
typedef struct LEAF LEAF;
#endif

// tMempool / leaf::Mempool — the LEAF memory pool type.
// C++ callers use leaf::Mempool directly via <Mempool.h>, included before
// extern "C" below.
// C callers use the tMempool_* and mpool_* shims declared further below.
#ifdef __cplusplus
#include <Mempool.h>
extern "C" {
#endif

#ifndef __cplusplus

typedef enum LEAFErrorType {
    LEAFMempoolOverrun = 0,
    LEAFMempoolFragmentation,
    LEAFInvalidFree,
    LEAFErrorNil
} LEAFErrorType;

/*!
 * @defgroup tmempool tMempool
 * @ingroup mempool
 * @brief Memory pool for the allocation of LEAF objects.
 * @{
 */

// node of free list
typedef struct mpool_node_t {
    char *pool;                // memory pool field
    struct mpool_node_t *next; // next node pointer
    struct mpool_node_t *prev; // prev node pointer
    size_t size;
} mpool_node_t;

// Aligned size of a node header - a compile-time constant used throughout the allocator
#define MPOOL_HEADER_SIZE (((sizeof(mpool_node_t)) + (MPOOL_ALIGN_SIZE - 1)) & ~(size_t)(MPOOL_ALIGN_SIZE - 1))

typedef struct tMempool tMempool;

struct tMempool {
    tMempool *mempool;                                     // parent pool
    char *mpool;                                           // start of the mpool
    size_t usize;                                          // used size of the pool
    size_t msize;                                          // max size of the pool
    mpool_node_t *head;                                    // first node of memory pool free list
    int clearOnAllocation;                                 //!< Whether to zero memory on allocation.
    unsigned int allocCount;                               //!< Count of allocations from this pool.
    unsigned int freeCount;                                //!< Count of frees from this pool.
    int errorState[LEAFErrorNil];                          //!< Flags indicating which errors have occurred.
    void (*errorCallback)(tMempool *const, LEAFErrorType); //!< Callback for pool errors.
};

/*!￼￼￼
 @} */

void tMempool_init      (tMempool **const pool, char *memory, size_t size, LEAF *const leaf);
void tMempool_free      (tMempool **const pool);
void tMempool_initToPool(tMempool **const mp, char *memory, size_t size, tMempool **const mem);

void   mpool_create   (char *memory, size_t size, tMempool *pool);
char  *mpool_alloc    (size_t size, tMempool *pool);
char  *mpool_calloc   (size_t asize, tMempool *pool);
void   mpool_free     (char *ptr, tMempool *pool);
size_t mpool_get_size (tMempool *pool);
size_t mpool_get_used (tMempool *pool);

void   leaf_pool_init    (LEAF *const leaf, char *memory, size_t size);
char  *leaf_alloc        (LEAF *const leaf, size_t size);
char  *leaf_calloc       (LEAF *const leaf, size_t size);
void   leaf_free         (LEAF *const leaf, char *ptr);
size_t leaf_pool_get_size(LEAF *const leaf);
size_t leaf_pool_get_used(LEAF *const leaf);
char  *leaf_pool_get_pool(LEAF *const leaf);

#endif // !__cplusplus

#ifdef __cplusplus
} // extern "C"
#endif

#endif // LEAF_MPOOL_H_INCLUDED

//==============================================================================
