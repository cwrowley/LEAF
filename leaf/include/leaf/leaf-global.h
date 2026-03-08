/*
 ==============================================================================

 leaf-global.h
 Created: 24 Oct 2019 2:24:38pm
 Author:  Matthew Wang

 ==============================================================================
 */

#ifndef LEAF_GLOBAL_H_INCLUDED
#define LEAF_GLOBAL_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif

#include <leaf-mempool.h>
#include "leaf-config.h"
typedef struct tLookupTable tLookupTable;

#ifdef __cplusplus
} // extern "C"

namespace leaf {
#endif

/*!
 * @ingroup leaf
 * @brief Struct for an instance of LEAF.
 */

struct LEAF {
    ///@{
    Lfloat sampleRate;              //!< The current audio sample rate. Set with LEAF_setSampleRate().
    Lfloat invSampleRate;           //!< The inverse of the current sample rate.
    Lfloat twoPiTimesInvSampleRate; //!<  Two-pi times the inverse of the current sample rate.
    Lfloat (*random)(void);         //!< A pointer to the random() function provided on initialization.
    tMempool *mempool;              //!< The default LEAF mempool object.
    tMempool _internal_mempool;
    unsigned int uuid;
    tLookupTable *lfoRateTable;
    tLookupTable *envTimeTable;
    tLookupTable *resTable;

    ///@}

#ifdef __cplusplus
    LEAF(Lfloat sampleRate, char *memory, size_t memorySize);
    ~LEAF() = default;
#endif
};

#ifdef __cplusplus
} // namespace leaf
#endif

#endif // LEAF_GLOBAL_H_INCLUDED

//==============================================================================
