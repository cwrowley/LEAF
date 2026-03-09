/*
 ==============================================================================

 Cycle.h
 C++ class for the wavetable sine/cycle oscillator, replacing the C tCycle type.

 ==============================================================================
*/

#ifndef LEAF_CYCLE_H_INCLUDED
#define LEAF_CYCLE_H_INCLUDED

#include <leaf-global.h>
#include <Mempool.h>
#include <leaf-tables.h>

//==============================================================================

namespace leaf {

/*!
 @class Cycle
 @brief Wavetable sine/cycle oscillator.

 Replaces the C tCycle type.  Memory for each instance is allocated from a
 LEAF mempool; in-place instances (e.g. packed arrays inside another object)
 are constructed with a null mempool pointer and must not be freed individually.
*/
class Cycle {
public:
    /// Allocate from the default mempool of a LEAF instance.
    Cycle(LEAF *const leaf);

    /// Allocate from an explicit mempool.
    Cycle(LEAF *const leaf, Mempool &m);

    /// Trivial destructor — pool memory is released by tCycle_free().
    ~Cycle() = default;

    Mempool *mempool() const { return mempool_; }

    Lfloat tick();
    void setFreq(Lfloat freq);
    void setPhase(Lfloat phase);
    void setSampleRate(Lfloat sr);

private:
    Mempool  *mempool_;
    uint32_t  phase_;
    int32_t   inc_;
    Lfloat    freq_;
    Lfloat    invSampleRateTimesTwoTo32_;
    uint32_t  mask_;
};

} // namespace leaf

/// Backward-compatible C++ alias — existing code using tCycle continues to work.
typedef leaf::Cycle tCycle;

//==============================================================================
// C shims — keep the tCycle_* names so all C callers are unchanged.
//==============================================================================

extern "C" {

void   tCycle_init      (tCycle **const osc, LEAF *const leaf);
void   tCycle_initToPool(tCycle **const osc, LEAF *const leaf, leaf::Mempool **const mempool);
void   tCycle_free      (tCycle **const osc);

Lfloat tCycle_tick      (tCycle *const osc);
void   tCycle_setFreq   (tCycle *const osc, Lfloat freq);
void   tCycle_setPhase  (tCycle *const osc, Lfloat phase);
void   tCycle_setSampleRate(tCycle *const osc, Lfloat sr);

} // extern "C"

#endif // LEAF_CYCLE_H_INCLUDED
