#pragma once
#include "Core/Bitboard.hpp"
#include "Core/Move.hpp"

namespace Engine {
    enum Color { WHITE, BLACK, COLOR_COUNT };
    enum Piece { PAWN, KNIGHT, BISHOP, ROOK, QUEEN, KING, PIECE_TYPE_COUNT };

    class Board {
    public:
        U64 pieceBitboards[COLOR_COUNT][PIECE_TYPE_COUNT];
        U64 occupancies[COLOR_COUNT];
        U64 allPieces;
        
        Move lastMove;
        bool hasLastMove;

        Board();
        void initStandardBoard();
        void updateOccupancies();
        void printBoard();
        bool getPieceAt(int square, int& outColor, int& outPiece) const;
        void makeMove(Move move, int color);
    };
}