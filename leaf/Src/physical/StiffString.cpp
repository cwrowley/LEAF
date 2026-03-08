/*
 ==============================================================================

 StiffString.cpp

 ==============================================================================
*/

#if _WIN32 || _WIN64
#include "..\..\Inc\physical\StiffString.h"
#else
#include "../../Inc/physical/StiffString.h"
#endif

#ifdef ARM_MATH_CM7
#include <arm_math.h>
#endif

#include <new>

StiffString::StiffString(int numModes, LEAF *const leaf)
    : StiffString(numModes, leaf, leaf->mempool) {}

void StiffString_init(StiffString **const pm, int numModes, LEAF *const leaf) {
    tMempool *m = leaf->mempool;
    *pm = new (mpool_alloc(sizeof(StiffString), m)) StiffString(numModes, leaf);
}

StiffString::StiffString(int numModes, LEAF *const leaf, tMempool *m) {
    mempool_ = m;

    // initialize variables
    numModes_ = numModes;
    freqHz_ = 440.0f;
    stiffness_ = 0.001f;
    pluckPos_ = 0.2f;
    pickupPos_ = 0.3f;
    decay_ = 0.0001f;
    decayHighFreq_ = 0.0003f;
    muteDecay_ = 0.4f;
    sampleRate_ = leaf->sampleRate;
    twoPiTimesInvSampleRate_ = leaf->twoPiTimesInvSampleRate;
    nyquist_ = sampleRate_ * 0.5f;
    Lfloat lessThanNyquist = sampleRate_ * 0.4f;
    nyquistScalingFactor_ = 1.0f / (lessThanNyquist - nyquist_);
    amp_ = 0.0f;
    gainComp_ = 0.0f;

    // allocate memory
    oscs_ = (tCycle *)mpool_alloc(numModes * sizeof(tCycle), m);
    for (int i = 0; i < numModes; ++i) {
        tCycle_initInPlace(&oscs_[i], leaf);
    }
    amplitudes_ = (Lfloat *)mpool_alloc(numModes * sizeof(Lfloat), m);
    outputWeights_ = (Lfloat *)mpool_alloc(numModes * sizeof(Lfloat), m);
    decayScalar_ = (Lfloat *)mpool_alloc(numModes * sizeof(Lfloat), m);
    decayVal_ = (Lfloat *)mpool_alloc(numModes * sizeof(Lfloat), m);
    nyquistCoeff_ = (Lfloat *)mpool_alloc(numModes * sizeof(Lfloat), m);
    updateOutputWeights();
}

void StiffString_initToPool(StiffString **const pm, int numModes, LEAF *const leaf, tMempool **const mp) {
    tMempool *m = *mp;
    *pm = new (mpool_alloc(sizeof(StiffString), m)) StiffString(numModes, leaf, m);
}

StiffString::~StiffString() {
    mpool_free((char *)nyquistCoeff_, mempool_);
    mpool_free((char *)decayScalar_, mempool_);
    mpool_free((char *)decayVal_, mempool_);
    mpool_free((char *)amplitudes_, mempool_);
    mpool_free((char *)outputWeights_, mempool_);
    mpool_free((char *)oscs_, mempool_);
}

void StiffString_free(StiffString **const pm) {
    StiffString *p = *pm;
    tMempool *m = p->mempool();
    p->~StiffString();
    mpool_free((char *)p, m);
}

void StiffString::updateOscillators() {
    Lfloat kappa_sq = stiffness_ * stiffness_;
    Lfloat compensation = 0.0f;
    for (int i = 0; i < numModes_; ++i) {
        int n = i + 1;
        int n_sq = n * n;
        Lfloat sig = decay_ + decayHighFreq_ * n_sq;
        Lfloat w0 = n * (1.0f + 0.5f * kappa_sq * n_sq);
        Lfloat zeta = sig / w0;
        Lfloat w = w0 * (1.0f - 0.5f * zeta * zeta);
        if (i == 0) {
            compensation = 1.0f / w;
        }
        Lfloat testFreq = (freqHz_ * w);
        Lfloat nyquistTest = (testFreq - nyquist_) * nyquistScalingFactor_;
        nyquistCoeff_[i] = LEAF_clip(0.0f, nyquistTest, 1.0f);
        oscs_[i].setFreq(testFreq * compensation);
        Lfloat val = freqHz_ * sig;
        Lfloat r = fastExp4(-val * twoPiTimesInvSampleRate_);
        decayScalar_[i] = r * r;
    }
}

void StiffString_updateOscillators(StiffString *const p) { p->updateOscillators(); }

void StiffString::updateOutputWeights() {
    Lfloat x0 = pickupPos_ * PI;
    Lfloat totalGain = 0.0f;
    for (int i = 0; i < numModes_; ++i) {
#ifdef ARM_MATH_CM7
        outputWeights_[i] = arm_sin_f32((i + 1) * x0);
        totalGain += fabsf(outputWeights_[i]) * amplitudes_[i];
#else
        outputWeights_[i] = sinf((i + 1) * x0);
        totalGain += outputWeights_[i] * amplitudes_[i];
#endif
    }
    if (totalGain < 0.01f) {
        totalGain = 0.01f;
    }
    totalGain = LEAF_clip(0.01f, totalGain, 1.0f);
    gainComp_ = 1.0f / totalGain;
}

void StiffString_updateOutputWeights(StiffString *const p) { p->updateOutputWeights(); }

Lfloat StiffString::tick() {
    Lfloat sample = 0.0f;
    for (int i = 0; i < numModes_; ++i) {
        sample += oscs_[i].tick() * amplitudes_[i] * outputWeights_[i] * decayVal_[i] * nyquistCoeff_[i];
        decayVal_[i] *= decayScalar_[i] * muteDecay_;
    }
    return sample * amp_ * gainComp_;
}

Lfloat StiffString_tick(StiffString *const p) { return p->tick(); }

void StiffString::setStiffness(Lfloat newValue) {
    stiffness_ = LEAF_mapFromZeroToOneInput(newValue, 0.00f, 0.2f);
}

void StiffString_setStiffness(StiffString *const p, Lfloat newValue) { p->setStiffness(newValue); }

void StiffString::setPickupPos(Lfloat newValue) {
    pickupPos_ = LEAF_clip(0.01f, newValue, 0.99f);
    updateOutputWeights();
}

void StiffString_setPickupPos(StiffString *const p, Lfloat newValue) { p->setPickupPos(newValue); }

void StiffString::setPluckPos(Lfloat newValue) {
    pluckPos_ = LEAF_clip(0.01f, newValue, 0.99f);
    updateOutputWeights();
}

void StiffString_setPluckPos(StiffString *const p, Lfloat newValue) { p->setPluckPos(newValue); }

void StiffString::setFreq(Lfloat newFreq) {
    freqHz_ = newFreq;
    updateOscillators();
}

void StiffString_setFreq(StiffString *const p, Lfloat newFreq) { p->setFreq(newFreq); }

void StiffString::setDecay(Lfloat decay) {
    decay_ = decay;
    updateOscillators();
}

void StiffString_setDecay(StiffString *const p, Lfloat decay) { p->setDecay(decay); }

void StiffString::setDecayHighFreq(Lfloat decayHF) {
    decayHighFreq_ = decayHF;
    updateOscillators();
}

void StiffString_setDecayHighFreq(StiffString *const p, Lfloat decayHF) { p->setDecayHighFreq(decayHF); }

void StiffString::mute() {
    muteDecay_ = 0.99f;
}

void StiffString_mute(StiffString *const p) { p->mute(); }

void StiffString::pluck(Lfloat amp) {
    Lfloat x0 = pluckPos_ * PI;
    muteDecay_ = 1.0f;
    for (int i = 0; i < numModes_; ++i) {
        int n = i + 1;
        float denom = n * n * x0 * (PI - x0);
        if (denom < 0.001f) {
            denom = 0.001f;
        }
#ifdef ARM_MATH_CM7
        amplitudes_[i] = 2.0f * arm_sin_f32(x0 * n) / denom;
#else
        amplitudes_[i] = 2.0f * sinf(x0 * n) / denom;
#endif
        decayVal_[i] = 1.0f;
    }
    amp_ = amp;
    updateOutputWeights();
}

void StiffString_pluck(StiffString *const p, Lfloat amp) { p->pluck(amp); }

void StiffString::setSampleRate(Lfloat sr) {
    sampleRate_ = sr;
    twoPiTimesInvSampleRate_ = TWO_PI / sr;
}

void StiffString_setSampleRate(StiffString *const p, Lfloat sr) { p->setSampleRate(sr); }

void StiffString::setStiffnessNoUpdate(Lfloat newValue) {
    stiffness_ = LEAF_mapFromZeroToOneInput(newValue, 0.00f, 0.2f);
}

void StiffString_setStiffnessNoUpdate(StiffString *const p, Lfloat newValue) { p->setStiffnessNoUpdate(newValue); }

void StiffString::setPickupPosNoUpdate(Lfloat newValue) {
    pickupPos_ = LEAF_clip(0.01f, newValue, 0.99f);
}

void StiffString_setPickupPosNoUpdate(StiffString *const p, Lfloat newValue) { p->setPickupPosNoUpdate(newValue); }

void StiffString::setPluckPosNoUpdate(Lfloat newValue) {
    pluckPos_ = LEAF_clip(0.01f, newValue, 0.99f);
}

void StiffString_setPluckPosNoUpdate(StiffString *const p, Lfloat newValue) { p->setPluckPosNoUpdate(newValue); }

void StiffString::setFreqNoUpdate(Lfloat newFreq) {
    freqHz_ = newFreq;
}

void StiffString_setFreqNoUpdate(StiffString *const p, Lfloat newFreq) { p->setFreqNoUpdate(newFreq); }

void StiffString::setDecayNoUpdate(Lfloat decay) {
    decay_ = decay;
}

void StiffString_setDecayNoUpdate(StiffString *const p, Lfloat decay) { p->setDecayNoUpdate(decay); }

void StiffString::setDecayHighFreqNoUpdate(Lfloat decayHF) {
    decayHighFreq_ = decayHF;
}

void StiffString_setDecayHighFreqNoUpdate(StiffString *const p, Lfloat decayHF) { p->setDecayHighFreqNoUpdate(decayHF); }

void StiffString::pluckNoUpdate(Lfloat amp) {
    Lfloat x0 = pluckPos_ * 0.5f * PI;
    muteDecay_ = 1.0f;
    for (int i = 0; i < numModes_; ++i) {
        int n = i + 1;
        float denom = n * n * x0 * (PI - x0);
        if (denom < 0.001f) {
            denom = 0.001f;
        }
#ifdef ARM_MATH_CM7
        amplitudes_[i] = 2.0f * arm_sin_f32(x0 * n) / denom;
#else
        amplitudes_[i] = 2.0f * sinf(x0 * n) / denom;
#endif
        decayVal_[i] = 1.0f;
    }
    amp_ = amp;
}

void StiffString_pluckNoUpdate(StiffString *const p, Lfloat amp) { p->pluckNoUpdate(amp); }
