/*
 ==============================================================================

 StiffString.cpp

 ==============================================================================
*/

#include <physical/StiffString.h>

#ifdef ARM_MATH_CM7
#include <arm_math.h>
#endif

#include <new>

//==============================================================================
// Class method definitions
//==============================================================================

namespace leaf {

StiffString::StiffString(LeafContext ctx, int numModes)
    : PoolAllocated<StiffString>(ctx)
{
    LEAF &leaf = ctx.leaf;

    // initialize variables
    numModes_ = numModes;
    freqHz_ = 440.0f;
    stiffness_ = 0.001f;
    pluckPos_ = 0.2f;
    pickupPos_ = 0.3f;
    decay_ = 0.0001f;
    decayHighFreq_ = 0.0003f;
    muteDecay_ = 0.4f;
    sampleRate_ = leaf.sampleRate();
    twoPiTimesInvSampleRate_ = leaf.twoPiTimesInvSampleRate();
    nyquist_ = sampleRate_ * 0.5f;
    Lfloat lessThanNyquist = sampleRate_ * 0.4f;
    nyquistScalingFactor_ = 1.0f / (lessThanNyquist - nyquist_);
    amp_ = 0.0f;
    gainComp_ = 0.0f;

    // allocate memory
    oscs_ = (Cycle *)palloc(numModes * sizeof(Cycle));
    for (int i = 0; i < numModes; ++i) {
        new (&oscs_[i]) Cycle(ctx);
    }
    amplitudes_ = (Lfloat *)palloc(numModes * sizeof(Lfloat));
    outputWeights_ = (Lfloat *)palloc(numModes * sizeof(Lfloat));
    decayScalar_ = (Lfloat *)palloc(numModes * sizeof(Lfloat));
    decayVal_ = (Lfloat *)palloc(numModes * sizeof(Lfloat));
    nyquistCoeff_ = (Lfloat *)palloc(numModes * sizeof(Lfloat));
    updateOutputWeights();
}

StiffString::~StiffString() {
    pfree((char *)nyquistCoeff_);
    pfree((char *)decayScalar_);
    pfree((char *)decayVal_);
    pfree((char *)amplitudes_);
    pfree((char *)outputWeights_);
    pfree((char *)oscs_);
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

Lfloat StiffString::tick() {
    Lfloat sample = 0.0f;
    for (int i = 0; i < numModes_; ++i) {
        sample += oscs_[i].tick() * amplitudes_[i] * outputWeights_[i] * decayVal_[i] * nyquistCoeff_[i];
        decayVal_[i] *= decayScalar_[i] * muteDecay_;
    }
    return sample * amp_ * gainComp_;
}

void StiffString::setStiffness(Lfloat newValue) {
    stiffness_ = LEAF_mapFromZeroToOneInput(newValue, 0.00f, 0.2f);
}

void StiffString::setPickupPos(Lfloat newValue) {
    pickupPos_ = LEAF_clip(0.01f, newValue, 0.99f);
    updateOutputWeights();
}

void StiffString::setPluckPos(Lfloat newValue) {
    pluckPos_ = LEAF_clip(0.01f, newValue, 0.99f);
    updateOutputWeights();
}

void StiffString::setFreq(Lfloat newFreq) {
    freqHz_ = newFreq;
    updateOscillators();
}

void StiffString::setDecay(Lfloat decay) {
    decay_ = decay;
    updateOscillators();
}

void StiffString::setDecayHighFreq(Lfloat decayHF) {
    decayHighFreq_ = decayHF;
    updateOscillators();
}

void StiffString::mute() {
    muteDecay_ = 0.99f;
}

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

void StiffString::setSampleRate(Lfloat sr) {
    sampleRate_ = sr;
    twoPiTimesInvSampleRate_ = TWO_PI / sr;
}

void StiffString::setStiffnessNoUpdate(Lfloat newValue) {
    stiffness_ = LEAF_mapFromZeroToOneInput(newValue, 0.00f, 0.2f);
}

void StiffString::setPickupPosNoUpdate(Lfloat newValue) {
    pickupPos_ = LEAF_clip(0.01f, newValue, 0.99f);
}

void StiffString::setPluckPosNoUpdate(Lfloat newValue) {
    pluckPos_ = LEAF_clip(0.01f, newValue, 0.99f);
}

void StiffString::setFreqNoUpdate(Lfloat newFreq) {
    freqHz_ = newFreq;
}

void StiffString::setDecayNoUpdate(Lfloat decay) {
    decay_ = decay;
}

void StiffString::setDecayHighFreqNoUpdate(Lfloat decayHF) {
    decayHighFreq_ = decayHF;
}

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

} // namespace leaf

//==============================================================================
// C shims
//==============================================================================

void StiffString_init(leaf::StiffString **const pm, int numModes, LEAF *const leaf) {
    *pm = leaf::StiffString::create(*leaf, numModes);
}

void StiffString_initToPool(leaf::StiffString **const pm, int numModes, LEAF *const leaf, leaf::Mempool **const mp) {
    *pm = leaf::StiffString::create(*leaf, **mp, numModes);
}

void StiffString_free(leaf::StiffString **const pm) {
    leaf::StiffString::destroy(*pm);
}

Lfloat StiffString_tick(leaf::StiffString *const p)                              { return p->tick(); }
void StiffString_setStiffness(leaf::StiffString *const p, Lfloat v)              { p->setStiffness(v); }
void StiffString_setFreq(leaf::StiffString *const p, Lfloat v)                   { p->setFreq(v); }
void StiffString_pluck(leaf::StiffString *const p, Lfloat v)                     { p->pluck(v); }
void StiffString_setPickupPos(leaf::StiffString *const p, Lfloat v)              { p->setPickupPos(v); }
void StiffString_setPluckPos(leaf::StiffString *const p, Lfloat v)               { p->setPluckPos(v); }
void StiffString_setDecay(leaf::StiffString *const p, Lfloat v)                  { p->setDecay(v); }
void StiffString_setDecayHighFreq(leaf::StiffString *const p, Lfloat v)          { p->setDecayHighFreq(v); }
void StiffString_updateOscillators(leaf::StiffString *const p)                   { p->updateOscillators(); }
void StiffString_updateOutputWeights(leaf::StiffString *const p)                 { p->updateOutputWeights(); }
void StiffString_mute(leaf::StiffString *const p)                                { p->mute(); }
void StiffString_setSampleRate(leaf::StiffString *const p, Lfloat v)             { p->setSampleRate(v); }
void StiffString_setStiffnessNoUpdate(leaf::StiffString *const p, Lfloat v)      { p->setStiffnessNoUpdate(v); }
void StiffString_setFreqNoUpdate(leaf::StiffString *const p, Lfloat v)           { p->setFreqNoUpdate(v); }
void StiffString_pluckNoUpdate(leaf::StiffString *const p, Lfloat v)             { p->pluckNoUpdate(v); }
void StiffString_setPickupPosNoUpdate(leaf::StiffString *const p, Lfloat v)      { p->setPickupPosNoUpdate(v); }
void StiffString_setPluckPosNoUpdate(leaf::StiffString *const p, Lfloat v)       { p->setPluckPosNoUpdate(v); }
void StiffString_setDecayNoUpdate(leaf::StiffString *const p, Lfloat v)          { p->setDecayNoUpdate(v); }
void StiffString_setDecayHighFreqNoUpdate(leaf::StiffString *const p, Lfloat v)  { p->setDecayHighFreqNoUpdate(v); }
