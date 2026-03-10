/*
  ==============================================================================

    LEAFCore.c
    Created: 20 Jan 2017 12:08:14pm
    Author:  Michael R Mulshine

  ==============================================================================
*/

#include <leaf.h>

// Simple LCG random number generator — no stdlib dependency.
// Returns a value in [0, 1).
static uint32_t _leaf_lcg_state = 1664525u;
static Lfloat LEAF_defaultRandom(void) {
    _leaf_lcg_state = _leaf_lcg_state * 1664525u + 1013904223u;
    return (_leaf_lcg_state >> 8) * (1.0f / 16777216.0f);
}

// ============================================================================
// C++ constructor — primary initialisation path.
// ============================================================================

leaf::LEAF::LEAF(Lfloat sr, char *memory, size_t memorySize)
    : sampleRate_(sr)
    , invSampleRate_(1.0f / sr)
    , twoPiTimesInvSampleRate_(invSampleRate_ * TWO_PI)
    , random_(LEAF_defaultRandom)
    , _internal_mempool_(memory, memorySize)
    , mempool_(&_internal_mempool_)
    , uuid_(0)
    , lfoRateTable_(nullptr)
    , envTimeTable_(nullptr)
    , resTable_(nullptr) {}

// ============================================================================
// C shims — thin wrappers that call the C++ methods.
// ============================================================================

void LEAF_init(LEAF *const leaf, Lfloat sr, char *memory, size_t memorysize) {
    new (leaf) leaf::LEAF(sr, memory, memorysize);
}

void LEAF_setRandom(LEAF *const leaf, Lfloat (*random)(void)) {
    leaf->setRandom(random);
}

void LEAF_setSampleRate(LEAF *const leaf, Lfloat sampleRate) {
    leaf->setSampleRate(sampleRate);
}

Lfloat LEAF_getSampleRate(LEAF *const leaf) {
    return leaf->sampleRate();
}

void LEAF_setErrorCallback(LEAF *const leaf, void (*callback)(tMempool *const, LEAFErrorType)) {
    leaf->setErrorCallback(callback);
}

unsigned int getNextUuid(LEAF *leaf) {
    return leaf->nextUuid();
}
