#include <catch2/catch_test_macros.hpp>
#include <catch2/matchers/catch_matchers_floating_point.hpp>
#include <helpers/LEAFFixture.h>
#include <helpers/SignalAnalysis.h>

#include <cmath>

using namespace leaf;

// ============================================================================
// Smoke tests
// ============================================================================

TEST_CASE_METHOD(LEAFFixture, "Cycle tick() produces finite output at 440 Hz over 4096 samples",
                 "[smoke][Cycle]")
{
    Cycle c(leaf);
    c.setFreq(440.0f);
    CHECK(allFinite(c, 4096));
}

TEST_CASE_METHOD(LEAFFixture, "Cycle setPhase(0.25) does not crash and output stays finite",
                 "[smoke][Cycle]")
{
    Cycle c(leaf);
    c.setFreq(440.0f);
    advance(c, 128);
    c.setPhase(0.25f);
    CHECK(allFinite(c, 1024));
}

TEST_CASE_METHOD(LEAFFixture, "Cycle setSampleRate(48000) does not crash and output stays finite",
                 "[smoke][Cycle]")
{
    Cycle c(leaf);
    c.setFreq(440.0f);
    advance(c, 128);
    c.setSampleRate(48000.0f);
    CHECK(allFinite(c, 1024));
}

// ============================================================================
// Behavioural tests
// ============================================================================

TEST_CASE_METHOD(LEAFFixture, "Cycle RMS at 440 Hz is close to 1/sqrt(2)",
                 "[behaviour][Cycle]")
{
    Cycle c(leaf);
    c.setFreq(440.0f);

    float measured = rms(c, 44100);
    float expected = 1.0f / std::sqrt(2.0f); // ≈ 0.7071
    const float tolerance = 1e-4f;

    CHECK_THAT(measured, Catch::Matchers::WithinAbs(expected, tolerance));
}

TEST_CASE_METHOD(LEAFFixture, "Cycle zero-crossing rate matches frequency at 440 Hz",
                 "[behaviour][Cycle]")
{
    Cycle c(leaf);
    c.setFreq(440.0f);

    int crossings = zeroCrossings(c, 44100);

    // Expected: 2 * 440 = 880 crossings per 44100 samples.
    // Allow ±1%.
    int expected = 2 * 440;
    int lo = (expected * 99) / 100;
    int hi = (expected * 101) / 100;

    CHECK(crossings >= lo);
    CHECK(crossings <= hi);
}

TEST_CASE_METHOD(LEAFFixture, "Cycle doubling frequency roughly doubles zero-crossing rate",
                 "[behaviour][Cycle]")
{
    Cycle c(leaf);

    c.setFreq(220.0f);
    int crossings220 = zeroCrossings(c, 44100);

    c.setFreq(440.0f);
    int crossings440 = zeroCrossings(c, 44100);

    // Doubling the frequency should double the zero-crossing count.
    // Accept within ±0.5% of 2×: i.e. [1.99×, 2.01×] crossings220.
    CHECK(crossings440 >= (crossings220 * 199) / 100);
    CHECK(crossings440 <= (crossings220 * 201) / 100);
}

TEST_CASE_METHOD(LEAFFixture, "Cycle produces non-zero output at 440 Hz",
                 "[behaviour][Cycle]")
{
    Cycle c(leaf);
    c.setFreq(440.0f);
    CHECK(maxAbs(c, 1024) > 0.1f);
}

TEST_CASE_METHOD(LEAFFixture, "Cycle at zero frequency produces silence",
                 "[behaviour][Cycle]")
{
    Cycle c(leaf);
    c.setFreq(0.0f);
    CHECK(maxAbs(c, 256) < 1e-5f);
}
