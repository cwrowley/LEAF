#include <catch2/catch_test_macros.hpp>
#include <helpers/LEAFFixture.h>
#include <helpers/SignalAnalysis.h>

using namespace leaf;

static constexpr int NUM_MODES = 8;

// ============================================================================
// Smoke tests
// ============================================================================

TEST_CASE_METHOD(LEAFFixture, "StiffString tick() produces finite output after pluck over 4096 samples",
                 "[smoke][StiffString]")
{
    StiffString s(leaf, NUM_MODES);
    s.setFreq(220.0f);
    s.pluck(1.0f);
    CHECK(allFinite(s, 4096));
}

TEST_CASE_METHOD(LEAFFixture, "StiffString setters do not throw",
                 "[smoke][StiffString]")
{
    StiffString s(leaf, NUM_MODES);
    CHECK_NOTHROW(s.setFreq(220.0f));
    CHECK_NOTHROW(s.setStiffness(0.1f));
    CHECK_NOTHROW(s.setDecay(0.9f));
    CHECK_NOTHROW(s.setDecayHighFreq(0.95f));
    CHECK_NOTHROW(s.setPickupPos(0.5f));
    CHECK_NOTHROW(s.setPluckPos(0.5f));
    CHECK_NOTHROW(s.setSampleRate(44100.0f));
}

// ============================================================================
// Behavioural tests
// ============================================================================

TEST_CASE_METHOD(LEAFFixture, "StiffString produces non-zero output immediately after pluck",
                 "[behaviour][StiffString]")
{
    StiffString s(leaf, NUM_MODES);
    s.setFreq(220.0f);
    s.pluck(1.0f);
    CHECK(maxAbs(s, 512) > 0.01f);
}

TEST_CASE_METHOD(LEAFFixture, "StiffString amplitude decays over time",
                 "[behaviour][StiffString]")
{
    // First instance: measure RMS of first 512 samples after pluck.
    StiffString early(leaf, NUM_MODES);
    early.setFreq(220.0f);
    early.pluck(1.0f);
    float rmsEarly = rms(early, 512);

    // Second instance: advance ~1 second past the pluck, then measure RMS.
    StiffString late(leaf, NUM_MODES);
    late.setFreq(220.0f);
    late.pluck(1.0f);
    advance(late, 44100);
    float rmsLate = rms(late, 512);

    CHECK(rmsEarly > rmsLate);
}

TEST_CASE_METHOD(LEAFFixture, "StiffString mute() decays signal faster than no mute",
                 "[behaviour][StiffString]")
{
    // base: pluck, advance 512 samples, then measure RMS over next 1024 samples.
    StiffString base(leaf, NUM_MODES);
    base.setFreq(220.0f);
    base.pluck(1.0f);
    advance(base, 512);
    float rmsBase = rms(base, 1024);

    // muted: same setup but call mute() before measuring.
    StiffString muted(leaf, NUM_MODES);
    muted.setFreq(220.0f);
    muted.pluck(1.0f);
    advance(muted, 512);
    muted.mute();
    float rmsMuted = rms(muted, 1024);

    CHECK(rmsMuted < rmsBase);
}

TEST_CASE_METHOD(LEAFFixture, "StiffString mute() reaches near silence after 2000 samples",
                 "[behaviour][StiffString]")
{
    // muteDecay ≈ 0.99 per sample → 0.99^2000 ≈ 2e-9 decay factor.
    StiffString s(leaf, NUM_MODES);
    s.setFreq(220.0f);
    s.pluck(1.0f);
    s.mute();
    advance(s, 2000);
    float rmsSilence = rms(s, 256);
    CHECK(rmsSilence < 1e-4f);
}
