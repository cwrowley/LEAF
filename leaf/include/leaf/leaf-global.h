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

// Include Mempool.h before extern "C" so leaf::Mempool (and tMempool typedef)
// are defined before the LEAF class uses Mempool by value below.
#include <Mempool.h>
extern "C" {
#include <leaf-mempool.h>
#include "leaf-config.h"
typedef struct tLookupTable tLookupTable;
} // extern "C"

// TWO_PI may not be visible yet (leaf-math.h transitively includes us).
#ifndef TWO_PI
#define TWO_PI (6.28318530717958f)
#endif

// Forward-declare the C shim that needs private access.
extern "C" void leaf_pool_init(leaf::LEAF *const leaf, char *memory, size_t size);

namespace leaf {

/*!
 * @ingroup leaf
 * @brief Class for an instance of LEAF.
 *
 * All data members are private; use the inline accessor methods.
 * The C shims in leaf.h (LEAF_init, LEAF_setSampleRate, etc.) delegate
 * to these methods and remain the API for C code.
 */
class LEAF {
public:
    LEAF(Lfloat sampleRate, char *memory, size_t memorySize);
    ~LEAF() = default;

    // -------------------------------------------------------------------------
    // Sample rate — all three derived fields are updated atomically.
    // -------------------------------------------------------------------------
    inline void setSampleRate(Lfloat sr) {
        sampleRate_              = sr;
        invSampleRate_           = 1.0f / sr;
        twoPiTimesInvSampleRate_ = invSampleRate_ * TWO_PI;
    }
    inline Lfloat sampleRate()              const { return sampleRate_; }
    inline Lfloat invSampleRate()           const { return invSampleRate_; }
    inline Lfloat twoPiTimesInvSampleRate() const { return twoPiTimesInvSampleRate_; }

    // -------------------------------------------------------------------------
    // Random function
    // -------------------------------------------------------------------------
    inline void setRandom(Lfloat (*fn)(void))       { random_ = fn; }
    inline auto random() const -> Lfloat (*)(void)  { return random_; }

    // -------------------------------------------------------------------------
    // Memory pool
    // -------------------------------------------------------------------------
    inline Mempool *mempool() const { return mempool_; }

    // -------------------------------------------------------------------------
    // Error callback (delegates to the root mempool)
    // -------------------------------------------------------------------------
    inline void setErrorCallback(void (*cb)(Mempool *const, LEAFErrorType)) {
        mempool_->setErrorCallback(cb);
    }

    // -------------------------------------------------------------------------
    // UUID counter
    // -------------------------------------------------------------------------
    inline unsigned int nextUuid() { return ++uuid_; }

    // -------------------------------------------------------------------------
    // Optional lookup tables
    // -------------------------------------------------------------------------
    inline tLookupTable *lfoRateTable() const { return lfoRateTable_; }
    inline tLookupTable *envTimeTable() const { return envTimeTable_; }
    inline tLookupTable *resTable()     const { return resTable_; }

private:
    Lfloat       sampleRate_;
    Lfloat       invSampleRate_;
    Lfloat       twoPiTimesInvSampleRate_;
    Lfloat       (*random_)(void);
    Mempool     *mempool_;
    Mempool      _internal_mempool_;
    unsigned int uuid_;
    tLookupTable *lfoRateTable_;
    tLookupTable *envTimeTable_;
    tLookupTable *resTable_;

    // C shim that needs direct access to _internal_mempool_ and mempool_.
    friend void ::leaf_pool_init(leaf::LEAF *const, char *, size_t);
};

} // namespace leaf

#else // !__cplusplus — C struct (same memory layout as the C++ class above)

#include <leaf-mempool.h>
#include "leaf-config.h"
typedef struct tLookupTable tLookupTable;

/*!
 * @ingroup leaf
 * @brief Struct for an instance of LEAF (C-mode view; layout matches leaf::LEAF).
 */
struct LEAF {
    Lfloat    sampleRate;              //!< The current audio sample rate.
    Lfloat    invSampleRate;           //!< The inverse of the current sample rate.
    Lfloat    twoPiTimesInvSampleRate; //!< Two-pi times the inverse of the current sample rate.
    Lfloat  (*random)(void);           //!< Random function in [0, 1).
    tMempool *mempool;                 //!< The default LEAF mempool pointer.
    tMempool  _internal_mempool;       //!< The embedded root mempool.
    unsigned int  uuid;
    tLookupTable *lfoRateTable;
    tLookupTable *envTimeTable;
    tLookupTable *resTable;
};

#endif // __cplusplus

#endif // LEAF_GLOBAL_H_INCLUDED

//==============================================================================
