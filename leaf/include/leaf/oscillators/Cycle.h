/*
 ==============================================================================

 Cycle.h
 C++ class for the wavetable sine/cycle oscillator, replacing the C tCycle type.

 ==============================================================================
*/

#ifndef LEAF_CYCLE_H_INCLUDED
#define LEAF_CYCLE_H_INCLUDED

#include <leaf-global.h>
#include <PoolAllocated.h>
#include <leaf-tables.h>

//==============================================================================

namespace leaf {

/*!
 @class Cycle
 @brief Wavetable sine/cycle oscillator.

 Replaces the C tCycle type.
*/
class Cycle : public PoolAllocated<Cycle> {
public:
    /// Construct from a LeafContext context (implicit from LEAF& for stack use).
    Cycle(LeafContext ctx);

    ~Cycle() = default;

    Lfloat tick();
    void setFreq(Lfloat freq);
    void setPhase(Lfloat phase);
    void setSampleRate(Lfloat sr);

private:
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
