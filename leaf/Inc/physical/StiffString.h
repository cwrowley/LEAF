/*
 ==============================================================================

 StiffString.h

 ==============================================================================
*/

#ifndef LEAF_STIFFSTRING_H_INCLUDED
#define LEAF_STIFFSTRING_H_INCLUDED

#include "leaf-global.h"
#include "leaf-mempool.h"
#include "leaf-oscillators.h"
#include "leaf-math.h"

//==============================================================================

class StiffString {
public:
    StiffString(int numModes, LEAF *const leaf);
    StiffString(int numModes, LEAF *const leaf, tMempool *m);
    ~StiffString();

    tMempool *mempool() const { return mempool_; }

    Lfloat tick();
    void setStiffness(Lfloat newValue);
    void setFreq(Lfloat newFreq);
    void pluck(Lfloat amp);
    void setPickupPos(Lfloat pickuppos);
    void setPluckPos(Lfloat pluckpos);
    void setDecay(Lfloat decay);
    void setDecayHighFreq(Lfloat decayHF);
    void updateOscillators();
    void updateOutputWeights();
    void mute();
    void setSampleRate(Lfloat sr);
    void setStiffnessNoUpdate(Lfloat newValue);
    void setFreqNoUpdate(Lfloat newFreq);
    void pluckNoUpdate(Lfloat amp);
    void setPickupPosNoUpdate(Lfloat pickuppos);
    void setPluckPosNoUpdate(Lfloat pluckpos);
    void setDecayNoUpdate(Lfloat decay);
    void setDecayHighFreqNoUpdate(Lfloat decayHF);

private:
    tMempool *mempool_;
    int numModes_;
    tCycle *oscs_;
    Lfloat *amplitudes_;
    Lfloat *outputWeights_;
    Lfloat freqHz_;
    Lfloat stiffness_;
    Lfloat pluckPos_;
    Lfloat pickupPos_;
    Lfloat decay_;
    Lfloat decayHighFreq_;
    Lfloat sampleRate_;
    Lfloat twoPiTimesInvSampleRate_;
    Lfloat *decayScalar_;
    Lfloat *decayVal_;
    Lfloat *nyquistCoeff_;
    Lfloat nyquist_;
    Lfloat nyquistScalingFactor_;
    Lfloat muteDecay_;
    Lfloat amp_;
    Lfloat gainComp_;
};

// C shims
#ifdef __cplusplus
extern "C" {
#endif

void StiffString_init(StiffString **const, int numModes, LEAF *const leaf);
void StiffString_initToPool(StiffString **const, int numModes, LEAF *const leaf, tMempool **const);
void StiffString_free(StiffString **const);

Lfloat StiffString_tick(StiffString *const);
void StiffString_setStiffness(StiffString *const, Lfloat newValue);
void StiffString_setFreq(StiffString *const, Lfloat newFreq);
void StiffString_pluck(StiffString *const, Lfloat amp);
void StiffString_setPickupPos(StiffString *const, Lfloat pickuppos);
void StiffString_setPluckPos(StiffString *const, Lfloat pluckpos);
void StiffString_setDecay(StiffString *const, Lfloat decay);
void StiffString_setDecayHighFreq(StiffString *const, Lfloat decayHF);
void StiffString_updateOscillators(StiffString *const pm);
void StiffString_updateOutputWeights(StiffString *const pm);
void StiffString_mute(StiffString *const pm);
void StiffString_setSampleRate(StiffString *const, Lfloat sr);
void StiffString_setStiffnessNoUpdate(StiffString *const, Lfloat newValue);
void StiffString_setFreqNoUpdate(StiffString *const, Lfloat newFreq);
void StiffString_pluckNoUpdate(StiffString *const, Lfloat amp);
void StiffString_setPickupPosNoUpdate(StiffString *const, Lfloat pickuppos);
void StiffString_setPluckPosNoUpdate(StiffString *const, Lfloat pluckpos);
void StiffString_setDecayNoUpdate(StiffString *const, Lfloat decay);
void StiffString_setDecayHighFreqNoUpdate(StiffString *const, Lfloat decayHF);

#ifdef __cplusplus
}
#endif

#endif // LEAF_STIFFSTRING_H_INCLUDED
