#include <catch2/catch_test_macros.hpp>
#include <helpers/LEAFFixture.h>

using namespace leaf;

static constexpr int NUM_MODES = 4;

// ============================================================================
// Cycle allocation tests
// ============================================================================

TEST_CASE_METHOD(LEAFFixture, "Cycle stack allocation does not touch either pool",
                 "[allocation][Cycle]")
{
    size_t usedBefore = leaf.mempool()->getUsed();

    {
        Cycle c(leaf);
        // The object itself lives on the C++ call stack, not in any pool.
        CHECK_FALSE(inEitherPool(&c));
        // Cycle has no internal dynamic allocations, so pool usage is unchanged.
        CHECK(leaf.mempool()->getUsed() == usedBefore);
    }
}

TEST_CASE_METHOD(LEAFFixture, "Cycle::create() allocates in default pool; destroy() nulls pointer",
                 "[allocation][Cycle]")
{
    size_t usedBefore = leaf.mempool()->getUsed();

    Cycle* c = Cycle::create(leaf);
    CHECK(inDefaultPool(c));
    CHECK(leaf.mempool()->getUsed() > usedBefore);

    Cycle::destroy(c);
    CHECK(c == nullptr);
}

TEST_CASE_METHOD(LEAFFixture, "Cycle::create() with secondary pool allocates there; default pool unchanged",
                 "[allocation][Cycle]")
{
    size_t defaultUsedBefore   = leaf.mempool()->getUsed();
    size_t secondaryUsedBefore = secondary.getUsed();

    Cycle* c = Cycle::create(leaf, secondary);
    CHECK(inSecondaryPool(c));
    CHECK(leaf.mempool()->getUsed() == defaultUsedBefore);
    CHECK(secondary.getUsed() > secondaryUsedBefore);

    Cycle::destroy(c);
    CHECK(c == nullptr);
}

TEST_CASE_METHOD(LEAFFixture, "tCycle_init allocates in default pool; tCycle_free nulls pointer",
                 "[allocation][Cycle][shim]")
{
    size_t usedBefore = leaf.mempool()->getUsed();

    tCycle* c = nullptr;
    tCycle_init(&c, &leaf);
    CHECK(inDefaultPool(c));
    CHECK(leaf.mempool()->getUsed() > usedBefore);

    tCycle_free(&c);
    CHECK(c == nullptr);
}

TEST_CASE_METHOD(LEAFFixture, "tCycle_initToPool allocates in secondary pool; default pool unchanged",
                 "[allocation][Cycle][shim]")
{
    size_t defaultUsedBefore = leaf.mempool()->getUsed();

    Mempool* secPtr = &secondary;
    tCycle*  c      = nullptr;
    tCycle_initToPool(&c, &leaf, &secPtr);
    CHECK(inSecondaryPool(c));
    CHECK(leaf.mempool()->getUsed() == defaultUsedBefore);

    tCycle_free(&c);
    CHECK(c == nullptr);
}

// ============================================================================
// StiffString allocation tests
// ============================================================================

TEST_CASE_METHOD(LEAFFixture, "StiffString stack allocation: object not in pool, but default pool usage increases",
                 "[allocation][StiffString]")
{
    size_t usedBefore = leaf.mempool()->getUsed();

    {
        StiffString s(leaf, NUM_MODES);
        // The StiffString object itself is on the stack.
        CHECK_FALSE(inEitherPool(&s));
        // Internal oscillator/weight arrays are palloc'd from the default pool.
        CHECK(leaf.mempool()->getUsed() > usedBefore);
    }
    // Stack destructor should release internal arrays.
}

TEST_CASE_METHOD(LEAFFixture, "StiffString::create() allocates in default pool; destroy() nulls pointer",
                 "[allocation][StiffString]")
{
    size_t usedBefore = leaf.mempool()->getUsed();

    StiffString* s = StiffString::create(leaf, NUM_MODES);
    CHECK(inDefaultPool(s));
    CHECK(leaf.mempool()->getUsed() > usedBefore);

    StiffString::destroy(s);
    CHECK(s == nullptr);
}

TEST_CASE_METHOD(LEAFFixture, "StiffString::create() with secondary pool allocates there; default pool unchanged",
                 "[allocation][StiffString]")
{
    size_t defaultUsedBefore = leaf.mempool()->getUsed();

    StiffString* s = StiffString::create(leaf, secondary, NUM_MODES);
    CHECK(inSecondaryPool(s));
    CHECK(leaf.mempool()->getUsed() == defaultUsedBefore);

    StiffString::destroy(s);
    CHECK(s == nullptr);
}

TEST_CASE_METHOD(LEAFFixture, "StiffString_init allocates in default pool; StiffString_free nulls pointer",
                 "[allocation][StiffString][shim]")
{
    size_t usedBefore = leaf.mempool()->getUsed();

    StiffString* s = nullptr;
    StiffString_init(&s, NUM_MODES, &leaf);
    CHECK(inDefaultPool(s));
    CHECK(leaf.mempool()->getUsed() > usedBefore);

    StiffString_free(&s);
    CHECK(s == nullptr);
}

TEST_CASE_METHOD(LEAFFixture, "StiffString_initToPool allocates in secondary pool; default pool unchanged",
                 "[allocation][StiffString][shim]")
{
    size_t defaultUsedBefore = leaf.mempool()->getUsed();

    Mempool*     secPtr = &secondary;
    StiffString* s      = nullptr;
    StiffString_initToPool(&s, NUM_MODES, &leaf, &secPtr);
    CHECK(inSecondaryPool(s));
    CHECK(leaf.mempool()->getUsed() == defaultUsedBefore);

    StiffString_free(&s);
    CHECK(s == nullptr);
}
