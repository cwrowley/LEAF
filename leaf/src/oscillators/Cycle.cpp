/*
 ==============================================================================

 Cycle.cpp

 ==============================================================================
*/

#include <oscillators/Cycle.h>
#include <leaf-config.h>

#ifdef ARM_MATH_CM7
#include <arm_math.h>
#endif

#include <new>

#if LEAF_INCLUDE_SINE_TABLE

//==============================================================================
// Class method definitions
//==============================================================================

namespace leaf {

Cycle::Cycle(LEAF *const leaf) : Cycle(leaf, leaf->mempool) {}

Cycle::Cycle(LEAF *const leaf, Mempool *m) {
    mempool_ = m;
    inc_     = 0;
    phase_   = 0;
    freq_    = 0.0f;
    invSampleRateTimesTwoTo32_ = leaf->invSampleRate * TWO_TO_32;
    mask_    = SINE_TABLE_SIZE - 1;
}

Lfloat Cycle::tick() {
    phase_ += inc_;
    uint32_t idx      = phase_ >> 21;           // 11-bit table index
    uint32_t tempFrac = phase_ & 2097151u;       // fractional remainder (2^21 - 1)
    Lfloat   samp0    = __leaf_table_sinewave[idx];
    Lfloat   samp1    = __leaf_table_sinewave[(idx + 1) & mask_];
    return samp0 + (samp1 - samp0) * ((Lfloat)tempFrac * 0.000000476837386f);
}

void Cycle::setFreq(Lfloat freq) {
    freq_ = freq;
    inc_  = freq * invSampleRateTimesTwoTo32_;
}

void Cycle::setPhase(Lfloat phase) {
    int i = (int)phase;
    phase -= i;
    phase_ = (uint32_t)(phase * TWO_TO_32);
}

void Cycle::setSampleRate(Lfloat sr) {
    invSampleRateTimesTwoTo32_ = (1.0f / sr) * TWO_TO_32;
    setFreq(freq_);
}

} // namespace leaf

//==============================================================================
// C shims — names unchanged so all C callers require no modification.
//==============================================================================

void tCycle_init(tCycle **const cy, LEAF *const leaf) {
    tCycle_initToPool(cy, leaf, &leaf->mempool);  // leaf->mempool is leaf::Mempool*
}

void tCycle_initToPool(tCycle **const cy, LEAF *const leaf, leaf::Mempool **const mp) {
    leaf::Mempool *m = *mp;
    void *mem        = m->alloc(sizeof(leaf::Cycle));
    leaf::Cycle *c   = new (mem) leaf::Cycle(leaf, m);
    *cy = c;
}

void tCycle_initInPlace(tCycle *c, LEAF *const leaf) {
    // Construct in pre-allocated memory with a null mempool (not pool-managed).
    new (c) leaf::Cycle(leaf, (leaf::Mempool *)nullptr);
}

void tCycle_free(tCycle **const cy) {
    leaf::Cycle   *c = *cy;
    leaf::Mempool *m = c->mempool();
    c->~Cycle();
    m->free((char *)c);
}

Lfloat tCycle_tick(tCycle *const c)              { return c->tick(); }
void   tCycle_setFreq(tCycle *const c, Lfloat f) { c->setFreq(f); }
void   tCycle_setPhase(tCycle *const c, Lfloat p){ c->setPhase(p); }
void   tCycle_setSampleRate(tCycle *const c, Lfloat sr) { c->setSampleRate(sr); }

#endif // LEAF_INCLUDE_SINE_TABLE
