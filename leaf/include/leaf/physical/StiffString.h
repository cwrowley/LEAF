/*
 ==============================================================================

 StiffString.h

 ==============================================================================
*/

#ifndef LEAF_STIFFSTRING_H_INCLUDED
#define LEAF_STIFFSTRING_H_INCLUDED

#include <leaf-global.h>
#include <Mempool.h>
#include <leaf-oscillators.h>
#include <leaf-math.h>

//==============================================================================

namespace leaf {

class StiffString {
public:
    StiffString(int numModes, LEAF *const leaf);
    StiffString(int numModes, LEAF *const leaf, Mempool *m);
    ~StiffString();

    Mempool *mempool() const { return mempool_; }

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
    Mempool  *mempool_;
    int numModes_;
    Cycle *oscs_;
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

} // namespace leaf

// C shims
extern "C" {

void StiffString_init(leaf::StiffString **const, int numModes, LEAF *const leaf);
void StiffString_initToPool(leaf::StiffString **const, int numModes, LEAF *const leaf, leaf::Mempool **const);
void StiffString_free(leaf::StiffString **const);

Lfloat StiffString_tick(leaf::StiffString *const);
void StiffString_setStiffness(leaf::StiffString *const, Lfloat newValue);
void StiffString_setFreq(leaf::StiffString *const, Lfloat newFreq);
void StiffString_pluck(leaf::StiffString *const, Lfloat amp);
void StiffString_setPickupPos(leaf::StiffString *const, Lfloat pickuppos);
void StiffString_setPluckPos(leaf::StiffString *const, Lfloat pluckpos);
void StiffString_setDecay(leaf::StiffString *const, Lfloat decay);
void StiffString_setDecayHighFreq(leaf::StiffString *const, Lfloat decayHF);
void StiffString_updateOscillators(leaf::StiffString *const pm);
void StiffString_updateOutputWeights(leaf::StiffString *const pm);
void StiffString_mute(leaf::StiffString *const pm);
void StiffString_setSampleRate(leaf::StiffString *const, Lfloat sr);
void StiffString_setStiffnessNoUpdate(leaf::StiffString *const, Lfloat newValue);
void StiffString_setFreqNoUpdate(leaf::StiffString *const, Lfloat newFreq);
void StiffString_pluckNoUpdate(leaf::StiffString *const, Lfloat amp);
void StiffString_setPickupPosNoUpdate(leaf::StiffString *const, Lfloat pickuppos);
void StiffString_setPluckPosNoUpdate(leaf::StiffString *const, Lfloat pluckpos);
void StiffString_setDecayNoUpdate(leaf::StiffString *const, Lfloat decay);
void StiffString_setDecayHighFreqNoUpdate(leaf::StiffString *const, Lfloat decayHF);

} // extern "C"

#endif // LEAF_STIFFSTRING_H_INCLUDED
