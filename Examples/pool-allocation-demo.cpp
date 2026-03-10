/*
 ==============================================================================

 pool-allocation-demo.cpp

 Demonstrates and verifies the allocation strategies for LEAF C++ objects:
   1. Stack allocation
   2. Default mempool (embedded in the LEAF instance)  — C++ factory
   3. Explicit secondary mempool                        — C++ factory
   4. Default mempool                                   — C shim (legacy)
   5. Explicit secondary mempool                        — C shim (legacy)

 For each strategy the object's address is printed and checked against the
 known memory regions so placement can be confirmed at a glance.  Pool usage
 is also tracked before and after each allocation to show that the right pool
 is being charged.

 ==============================================================================
*/

#include <leaf.h>
#include <oscillators/Cycle.h>
#include <physical/StiffString.h>

#include <cstdio>
#include <cstdint>

using namespace leaf;

//==============================================================================
// Static memory regions
//==============================================================================

static const size_t MAIN_POOL_SIZE      = 64 * 1024;   // 64 KB
static const size_t SECONDARY_POOL_SIZE = 32 * 1024;   // 32 KB

static char mainMem     [MAIN_POOL_SIZE];
static char secondaryMem[SECONDARY_POOL_SIZE];

//==============================================================================
// Helpers
//==============================================================================

// Returns a string naming which memory region ptr falls in.
static const char *locate(const void *ptr)
{
    const char *p = static_cast<const char *>(ptr);
    if (p >= mainMem      && p < mainMem      + MAIN_POOL_SIZE)      return "default pool";
    if (p >= secondaryMem && p < secondaryMem + SECONDARY_POOL_SIZE) return "secondary pool";
    return "stack / static";
}

// Prints an object's address and the region it belongs to.
static void reportAddr(const char *label, const void *ptr)
{
    printf("    %-36s  %p  [%s]\n", label, ptr, locate(ptr));
}

// Prints current pool usage.
static void reportUsage(const char *label, const Mempool &pool)
{
    printf("    %-36s  %zu / %zu bytes used\n", label, pool.getUsed(), pool.getSize());
}

//==============================================================================
// Cycle demo
//==============================================================================

static void demoCycle(LEAF &leaf, Mempool &secondary)
{
    printf("\n=== Cycle ===\n");

    // ------------------------------------------------------------------
    // 1. Stack allocation — the Cycle object itself lives on the stack.
    //    Cycle has no internal dynamic allocations, so the default pool
    //    is not touched.
    // ------------------------------------------------------------------
    printf("\n  1. Stack allocation\n");
    {
        size_t usedBefore = leaf.mempool()->getUsed();

        Cycle c(leaf);           // LEAF& implicitly converts to LeafInit
        c.setFreq(440.0f);

        reportAddr("Cycle object", &c);
        printf("    default pool used: %zu -> %zu bytes  (no change)\n",
               usedBefore, leaf.mempool()->getUsed());
        printf("    tick() = %+.6f\n", c.tick());
    }   // c destroyed automatically here

    // ------------------------------------------------------------------
    // 2. Default-mempool allocation — object lives in the main pool.
    // ------------------------------------------------------------------
    printf("\n  2. Default-mempool allocation\n");
    {
        size_t usedBefore = leaf.mempool()->getUsed();

        Cycle *c = Cycle::create(leaf);
        c->setFreq(440.0f);

        reportAddr("Cycle object", c);
        printf("    default pool used: %zu -> %zu bytes\n",
               usedBefore, leaf.mempool()->getUsed());
        printf("    tick() = %+.6f\n", c->tick());

        Cycle::destroy(c);
        printf("    after destroy: ptr == nullptr? %s\n", c == nullptr ? "yes" : "no");
    }

    // ------------------------------------------------------------------
    // 3. Secondary-mempool allocation — object lives in the secondary pool.
    // ------------------------------------------------------------------
    printf("\n  3. Secondary-mempool allocation\n");
    {
        size_t mainBefore = leaf.mempool()->getUsed();
        size_t secBefore  = secondary.getUsed();

        Cycle *c = Cycle::create(leaf, secondary);
        c->setFreq(440.0f);

        reportAddr("Cycle object", c);
        printf("    default pool used:    %zu -> %zu bytes  (no change)\n",
               mainBefore, leaf.mempool()->getUsed());
        printf("    secondary pool used:  %zu -> %zu bytes\n",
               secBefore, secondary.getUsed());
        printf("    tick() = %+.6f\n", c->tick());

        Cycle::destroy(c);
        printf("    after destroy: ptr == nullptr? %s\n", c == nullptr ? "yes" : "no");
    }

    // ------------------------------------------------------------------
    // 4. C shim — default mempool (legacy interface).
    // ------------------------------------------------------------------
    printf("\n  4. C shim — default-mempool allocation\n");
    {
        size_t usedBefore = leaf.mempool()->getUsed();

        tCycle *c;
        tCycle_init(&c, &leaf);
        tCycle_setFreq(c, 440.0f);

        reportAddr("Cycle object", c);
        printf("    default pool used: %zu -> %zu bytes\n",
               usedBefore, leaf.mempool()->getUsed());
        printf("    tick() = %+.6f\n", tCycle_tick(c));

        tCycle_free(&c);
        printf("    after free: ptr == nullptr? %s\n", c == nullptr ? "yes" : "no");
    }

    // ------------------------------------------------------------------
    // 5. C shim — secondary mempool (legacy interface).
    //    tCycle_initToPool takes a Mempool**, so we pass the address of
    //    a Mempool* pointer.
    // ------------------------------------------------------------------
    printf("\n  5. C shim — secondary-mempool allocation\n");
    {
        size_t mainBefore = leaf.mempool()->getUsed();
        size_t secBefore  = secondary.getUsed();

        tCycle *c;
        Mempool *secPtr = &secondary;
        tCycle_initToPool(&c, &leaf, &secPtr);
        tCycle_setFreq(c, 440.0f);

        reportAddr("Cycle object", c);
        printf("    default pool used:    %zu -> %zu bytes  (no change)\n",
               mainBefore, leaf.mempool()->getUsed());
        printf("    secondary pool used:  %zu -> %zu bytes\n",
               secBefore, secondary.getUsed());
        printf("    tick() = %+.6f\n", tCycle_tick(c));

        tCycle_free(&c);
        printf("    after free: ptr == nullptr? %s\n", c == nullptr ? "yes" : "no");
    }
}

//==============================================================================
// StiffString demo
//==============================================================================

static void demoStiffString(LEAF &leaf, Mempool &secondary)
{
    printf("\n=== StiffString ===\n");

    const int NUM_MODES = 4;

    // ------------------------------------------------------------------
    // 1. Stack allocation — the StiffString object itself is on the stack,
    //    but its internal arrays (oscs_, amplitudes_, etc.) are palloc'd
    //    from the default mempool via the LeafInit context.
    // ------------------------------------------------------------------
    printf("\n  1. Stack allocation\n");
    {
        size_t usedBefore = leaf.mempool()->getUsed();

        StiffString s(leaf, NUM_MODES);
        s.setFreq(220.0f);
        s.pluck(1.0f);

        reportAddr("StiffString object (stack)", &s);
        printf("    default pool used: %zu -> %zu bytes  (internal arrays)\n",
               usedBefore, leaf.mempool()->getUsed());
        printf("    tick() = %+.6f\n", s.tick());
    }   // destructor calls pfree() on all internal arrays

    // ------------------------------------------------------------------
    // 2. Default-mempool allocation — object and internal arrays are all
    //    in the main pool.
    // ------------------------------------------------------------------
    printf("\n  2. Default-mempool allocation\n");
    {
        size_t usedBefore = leaf.mempool()->getUsed();

        StiffString *s = StiffString::create(leaf, NUM_MODES);
        s->setFreq(220.0f);
        s->pluck(1.0f);

        reportAddr("StiffString object", s);
        printf("    default pool used: %zu -> %zu bytes\n",
               usedBefore, leaf.mempool()->getUsed());
        printf("    tick() = %+.6f\n", s->tick());

        StiffString::destroy(s);
        printf("    after destroy: ptr == nullptr? %s\n", s == nullptr ? "yes" : "no");
    }

    // ------------------------------------------------------------------
    // 3. Secondary-mempool allocation — object and all internal arrays
    //    land in the secondary pool; the main pool is untouched.
    // ------------------------------------------------------------------
    printf("\n  3. Secondary-mempool allocation\n");
    {
        size_t mainBefore = leaf.mempool()->getUsed();
        size_t secBefore  = secondary.getUsed();

        StiffString *s = StiffString::create(leaf, secondary, NUM_MODES);
        s->setFreq(220.0f);
        s->pluck(1.0f);

        reportAddr("StiffString object", s);
        printf("    default pool used:    %zu -> %zu bytes  (no change)\n",
               mainBefore, leaf.mempool()->getUsed());
        printf("    secondary pool used:  %zu -> %zu bytes\n",
               secBefore, secondary.getUsed());
        printf("    tick() = %+.6f\n", s->tick());

        StiffString::destroy(s);
        printf("    after destroy: ptr == nullptr? %s\n", s == nullptr ? "yes" : "no");
    }

    // ------------------------------------------------------------------
    // 4. C shim — default mempool (legacy interface).
    // ------------------------------------------------------------------
    printf("\n  4. C shim — default-mempool allocation\n");
    {
        size_t usedBefore = leaf.mempool()->getUsed();

        leaf::StiffString *s;
        StiffString_init(&s, NUM_MODES, &leaf);
        s->setFreq(220.0f);
        s->pluck(1.0f);

        reportAddr("StiffString object", s);
        printf("    default pool used: %zu -> %zu bytes\n",
               usedBefore, leaf.mempool()->getUsed());
        printf("    tick() = %+.6f\n", StiffString_tick(s));

        StiffString_free(&s);
        printf("    after free: ptr == nullptr? %s\n", s == nullptr ? "yes" : "no");
    }

    // ------------------------------------------------------------------
    // 5. C shim — secondary mempool (legacy interface).
    // ------------------------------------------------------------------
    printf("\n  5. C shim — secondary-mempool allocation\n");
    {
        size_t mainBefore = leaf.mempool()->getUsed();
        size_t secBefore  = secondary.getUsed();

        leaf::StiffString *s;
        Mempool *secPtr = &secondary;
        StiffString_initToPool(&s, NUM_MODES, &leaf, &secPtr);
        s->setFreq(220.0f);
        s->pluck(1.0f);

        reportAddr("StiffString object", s);
        printf("    default pool used:    %zu -> %zu bytes  (no change)\n",
               mainBefore, leaf.mempool()->getUsed());
        printf("    secondary pool used:  %zu -> %zu bytes\n",
               secBefore, secondary.getUsed());
        printf("    tick() = %+.6f\n", StiffString_tick(s));

        StiffString_free(&s);
        printf("    after free: ptr == nullptr? %s\n", s == nullptr ? "yes" : "no");
    }
}

//==============================================================================
// main
//==============================================================================

int main()
{
    // Initialise LEAF with the main pool.
    LEAF leaf(44100.0f, mainMem, MAIN_POOL_SIZE);

    // Set up a secondary pool backed by its own static buffer.
    Mempool secondary(secondaryMem, SECONDARY_POOL_SIZE);

    printf("Memory map:\n");
    printf("  default pool:    %p .. %p  (%zu bytes)\n",
           (void *)mainMem,      (void *)(mainMem      + MAIN_POOL_SIZE),      MAIN_POOL_SIZE);
    printf("  secondary pool:  %p .. %p  (%zu bytes)\n",
           (void *)secondaryMem, (void *)(secondaryMem + SECONDARY_POOL_SIZE), SECONDARY_POOL_SIZE);

    demoCycle(leaf, secondary);
    demoStiffString(leaf, secondary);

    printf("\nDone.\n");
    return 0;
}
