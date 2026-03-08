/** BEGIN_JUCE_MODULE_DECLARATION

 ID:            leaf
 vendor:
 version:        0.0.1
 name:
 description:
 website:
 license:

 dependencies:

 END_JUCE_MODULE_DECLARATION
 */
/*
 ==============================================================================

 leaf.h
 Created: 20 Jan 2017 12:07:26pm
 Author:  Michael R Mulshine

 ==============================================================================
 */

#ifndef LEAF_H_INCLUDED
#define LEAF_H_INCLUDED
#include "leaf-config.h"
#define LEAF_DEBUG 0

// #if LEAF_DEBUG
// #include "../TestPlugin/JuceLibraryCode/JuceHeader.h"
// #endif

#if _WIN32 || _WIN64

#include ".\Inc\leaf-global.h"
#include ".\Inc\leaf-math.h"
#include ".\Inc\leaf-mempool.h"
#include ".\Inc\leaf-tables.h"
#include ".\Inc\leaf-distortion.h"
#include ".\Inc\leaf-oscillators.h"
#include ".\Inc\leaf-filters.h"
#include ".\Inc\leaf-delay.h"
#include ".\Inc\leaf-reverb.h"
#include ".\Inc\leaf-effects.h"
#include ".\Inc\leaf-envelopes.h"
#include ".\Inc\leaf-dynamics.h"
#include ".\Inc\leaf-analysis.h"
#include ".\Inc\leaf-instruments.h"
#include ".\Inc\leaf-midi.h"
#include ".\Inc\leaf-sampling.h"
#include ".\Inc\leaf-physical.h"
#include ".\Inc\leaf-electrical.h"
#include ".\Inc\leaf-vocal.h"

#else

#include "./Inc/leaf-global.h"
#include "./Inc/leaf-math.h"
#include "./Inc/leaf-mempool.h"
#include "./Inc/leaf-tables.h"
#include "./Inc/leaf-distortion.h"
#include "./Inc/leaf-dynamics.h"
#include "./Inc/leaf-oscillators.h"
#include "./Inc/leaf-filters.h"
#include "./Inc/leaf-delay.h"
#include "./Inc/leaf-reverb.h"
#include "./Inc/leaf-effects.h"
#include "./Inc/leaf-envelopes.h"
#include "./Inc/leaf-analysis.h"
#include "./Inc/leaf-instruments.h"
#include "./Inc/leaf-midi.h"
#include "./Inc/leaf-sampling.h"
#include "./Inc/leaf-physical.h"
#include "./Inc/leaf-electrical.h"
#include "./Inc/leaf-vocal.h"

#endif

/*! @mainpage LEAF
 *
 * @section intro_sec Introduction
 *
 * LEAF (Lightweight Embedded Audio Framework) is a no-dependency C audio library intended for embedded systems.
 *
 * This documentation is a work in progress and will eventually include usage instructions, examples, and descriptions of LEAF objects and functions.
 */
/*!
 * @internal
 * @section install_sec Installation
 *
 * @subsection step1 Step 1: Opening the box
 *
 * etc...
 */

/*!
 @defgroup leaf LEAF
 @brief The structure of an instance of LEAF.
 @defgroup oscillators Oscillators
 @brief Oscillators and generators.
 @defgroup filters Filters
 @brief Filters.
 @defgroup delay Delay
 @brief Delays.
 @defgroup reverb Reverb
 @brief Reverbs.
 @defgroup distortion Distortion
 @brief Degradation and oversampling.
 @defgroup effects Effects
 @brief Vocoders, pitch shifters, and more.
 @defgroup envelopes Envelopes
 @brief Envelopes and smoothers.
 @defgroup dynamics Dynamics
 @brief Compression and levelling.
 @defgroup analysis Analysis
 @brief Signal analysis.
 @defgroup instruments Instruments
 @brief Instruments.
 @defgroup midi MIDI
 @brief MIDI handling.
 @defgroup sampling Sampling
 @brief Samplers.
 @defgroup physical Physical Models
 @brief String models and more.
 @defgroup electrical Electrical Models
 @brief Circuit models.
 @defgroup mempool Mempool
 @brief Memory allocation.
 @defgroup math Math
 @brief Math functions.
 @defgroup tables Tables
 @brief Wavetables, filter tables, minBLEP tables, and more.
 */

#ifdef __cplusplus
extern "C" {
#endif

/*!
 @ingroup leaf
 @{
 */

//! Initialize the LEAF instance.
/*!
 @param leaf A pointer to the LEAF instance to initialize.
 @param sampleRate The default sample rate for objects initialized to this LEAF instance.
 @param memory A pointer to the memory that will make up the default mempool.
 @param memorySize The size of that memory block in bytes.
 */
void LEAF_init(LEAF *const leaf, Lfloat sampleRate, char *memory, size_t memorySize);

//! Override the random number generator used by LEAF objects.
/*!
 @brief By default LEAF uses a fast internal LCG. Call this after LEAF_init
        to substitute a platform-specific or higher-quality generator.
        The function must return a value in [0, 1).
 @param leaf A pointer to the LEAF instance.
 @param random A pointer to the replacement random function.
 */
void LEAF_setRandom(LEAF *const leaf, Lfloat (*random)(void));

//! Set the sample rate of LEAF.
/*!
 @param sampleRate The new audio sample rate.
 */
void LEAF_setSampleRate(LEAF *const leaf, Lfloat sampleRate);

//! Get the sample rate of LEAF.
/*!
 @return The current sample rate as a Lfloat.
 */
Lfloat LEAF_getSampleRate(LEAF *const leaf);

//! The default (no-op) callback function for pool errors.
/*!
 @param pool The pool in which the error occurred.
 @param errorType The type of the error that has occurred.
 */
void LEAF_defaultErrorCallback(tMempool *const pool, LEAFErrorType errorType);

unsigned int getNextUuid(LEAF *leaf);

//! Set the error callback for the default LEAF mempool.
/*!
 @param callback A pointer to the callback function.
 */
void LEAF_setErrorCallback(LEAF *const leaf, void (*callback)(tMempool *const, LEAFErrorType));

/*! @} */

#ifdef __cplusplus
}
#endif

#endif // LEAF_H_INCLUDED
