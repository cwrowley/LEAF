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

void LEAF_init(LEAF *const leaf, Lfloat sr, char *memory, size_t memorysize) {
    leaf_pool_init(leaf, memory, memorysize);

    leaf->sampleRate = sr;
    leaf->invSampleRate = 1.0f / sr;
    leaf->twoPiTimesInvSampleRate = leaf->invSampleRate * TWO_PI;
    leaf->random = LEAF_defaultRandom;
    leaf->uuid = 0;
    leaf->lfoRateTable = NULL;
    leaf->envTimeTable = NULL;
    leaf->resTable = NULL;
}

void LEAF_setRandom(LEAF *const leaf, Lfloat (*random)(void)) {
    leaf->random = random;
}

void LEAF_setSampleRate(LEAF *const leaf, Lfloat sampleRate) {
    leaf->sampleRate = sampleRate;
    leaf->invSampleRate = 1.0f / sampleRate;
    leaf->twoPiTimesInvSampleRate = leaf->invSampleRate * TWO_PI;
}

Lfloat LEAF_getSampleRate(LEAF *const leaf) {
    return leaf->sampleRate;
}

void LEAF_defaultErrorCallback(tMempool *const pool, LEAFErrorType whichone) {
    // No-op placeholder; users can override with LEAF_setErrorCallback.
}

void LEAF_setErrorCallback(LEAF *const leaf, void (*callback)(tMempool *const, LEAFErrorType)) {
    leaf->_internal_mempool.errorCallback = callback;
}

unsigned int getNextUuid(LEAF *leaf) {
    return ++leaf->uuid;
}

LEAF::LEAF(Lfloat sr, char *memory, size_t memorySize) {
    LEAF_init(this, sr, memory, memorySize);
}
