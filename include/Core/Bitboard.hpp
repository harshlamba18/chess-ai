#pragma once
#include <cstdint>

typedef uint64_t U64;

namespace Engine {
    constexpr U64 notAFile  = 0xFEFEFEFEFEFEFEFEULL;
    constexpr U64 notHFile  = 0x7F7F7F7F7F7F7F7FULL;
    constexpr U64 notABFile = 0xFCFCFCFCFCFCFCFCULL;
    constexpr U64 notGHFile = 0x3F3F3F3F3F3F3F3FULL;

    void printBitboard(U64 bitboard);
}