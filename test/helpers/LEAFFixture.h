#pragma once

#include <leaf.h>
#include <oscillators/Cycle.h>
#include <physical/StiffString.h>

#include <cstddef>

/// Catch2-compatible fixture struct providing a fully-initialised LEAF instance
/// with a default 64 KB mempool and an additional 32 KB secondary mempool for
/// tests that exercise pool-selection logic.
struct LEAFFixture {
    static constexpr size_t MAIN_SIZE = 64 * 1024;
    static constexpr size_t SEC_SIZE  = 32 * 1024;

    char mainMem[MAIN_SIZE];
    char secMem[SEC_SIZE];

    leaf::LEAF    leaf { 44100.0f, mainMem, MAIN_SIZE };
    leaf::Mempool secondary;

    LEAFFixture()
    {
        secondary.initAsRoot(secMem, SEC_SIZE,
            [](leaf::Mempool*, LEAFErrorType) {});
    }

    /// True if @p ptr lies within the default (main) pool's memory region.
    bool inDefaultPool(const void* ptr) const
    {
        const char* p   = static_cast<const char*>(ptr);
        const char* lo  = mainMem;
        const char* hi  = mainMem + MAIN_SIZE;
        return p >= lo && p < hi;
    }

    /// True if @p ptr lies within the secondary pool's memory region.
    bool inSecondaryPool(const void* ptr) const
    {
        const char* p   = static_cast<const char*>(ptr);
        const char* lo  = secMem;
        const char* hi  = secMem + SEC_SIZE;
        return p >= lo && p < hi;
    }

    /// True if @p ptr lies within either pool.
    bool inEitherPool(const void* ptr) const
    {
        return inDefaultPool(ptr) || inSecondaryPool(ptr);
    }
};
