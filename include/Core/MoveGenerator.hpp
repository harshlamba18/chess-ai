#pragma once
#include <vector>
#include "Core/Board.hpp"
#include "Core/Move.hpp"

namespace Engine {
    class MoveGenerator {
    public:
        static std::vector<Move> generatePseudoLegalMoves(const Board& board, int color);
        static std::vector<Move> generateLegalMoves(const Board& board, int color);
        static bool isInCheck(const Board& board, int color);
        static bool isSquareAttacked(const Board& board, int square, int attackerColor);

    private:
        static void generatePawnMoves(const Board& board, int color, std::vector<Move>& moveList);
        static void generateKnightMoves(const Board& board, int color, std::vector<Move>& moveList);
        static void generateKingMoves(const Board& board, int color, std::vector<Move>& moveList);
        static void generateSlidingMoves(const Board& board, int color, std::vector<Move>& moveList);
    };
}