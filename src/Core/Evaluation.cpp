#include "Core/Evaluation.hpp"
#include "Core/BitOps.hpp"

namespace Engine {
    const int pawnValue = 100;
    const int knightValue = 300;
    const int bishopValue = 300;
    const int rookValue = 500;
    const int queenValue = 900;

    int Evaluation::evaluate(const Board& board) {
        int score = 0;
        
        score += countBits(board.pieceBitboards[WHITE][PAWN]) * pawnValue;
        score += countBits(board.pieceBitboards[WHITE][KNIGHT]) * knightValue;
        score += countBits(board.pieceBitboards[WHITE][BISHOP]) * bishopValue;
        score += countBits(board.pieceBitboards[WHITE][ROOK]) * rookValue;
        score += countBits(board.pieceBitboards[WHITE][QUEEN]) * queenValue;

        score -= countBits(board.pieceBitboards[BLACK][PAWN]) * pawnValue;
        score -= countBits(board.pieceBitboards[BLACK][KNIGHT]) * knightValue;
        score -= countBits(board.pieceBitboards[BLACK][BISHOP]) * bishopValue;
        score -= countBits(board.pieceBitboards[BLACK][ROOK]) * rookValue;
        score -= countBits(board.pieceBitboards[BLACK][QUEEN]) * queenValue;

        return score;
    }
}