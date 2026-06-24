#pragma once
#include "Core/Bitboard.hpp"

#ifdef _MSC_VER
#include <intrin.h>
#pragma intrinsic(_BitScanForward64)
#endif

namespace Engine {
    inline int getLSB(U64 bb) {
#ifdef _MSC_VER
        unsigned long index;
        _BitScanForward64(&index, bb);
        return static_cast<int>(index);
#else
        return __builtin_ctzll(bb);
#endif
    }

    inline int popLSB(U64& bb) {
        int lsb = getLSB(bb);
        bb &= bb - 1;
        return lsb;
    }

    inline int countBits(U64 bb) {
#ifdef _MSC_VER
        return static_cast<int>(__popcnt64(bb));
#else
        return __builtin_popcountll(bb);
#endif
    }
}