#include "Core/Bitboard.hpp"
#include <iostream>

namespace Engine {
    void printBitboard(U64 bitboard) {
        std::cout << "\n";
        for (int rank = 7; rank >= 0; rank--) {
            std::cout << (rank + 1) << "  ";
            for (int file = 0; file < 8; file++) {
                int square = rank * 8 + file;
                std::cout << ((bitboard >> square) & 1ULL) << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n   a b c d e f g h\n\n";
        std::cout << "Bitboard Value: " << bitboard << "ULL\n";
    }
}