#include "Core/Board.hpp"
#include <iostream>

namespace Engine
{

    Board::Board()
    {
        for (int c = 0; c < COLOR_COUNT; ++c)
        {
            occupancies[c] = 0ULL;
            for (int p = 0; p < PIECE_TYPE_COUNT; ++p)
            {
                pieceBitboards[c][p] = 0ULL;
            }
        }
        allPieces = 0ULL;
    }

    void Board::initStandardBoard()
    {
        pieceBitboards[WHITE][PAWN] = 0xFF00ULL;
        pieceBitboards[WHITE][KNIGHT] = 0x42ULL;
        pieceBitboards[WHITE][BISHOP] = 0x24ULL;
        pieceBitboards[WHITE][ROOK] = 0x81ULL;
        pieceBitboards[WHITE][QUEEN] = 0x08ULL;
        pieceBitboards[WHITE][KING] = 0x10ULL;

        pieceBitboards[BLACK][PAWN] = 0xFF00ULL << (5 * 8);
        pieceBitboards[BLACK][KNIGHT] = 0x42ULL << (7 * 8);
        pieceBitboards[BLACK][BISHOP] = 0x24ULL << (7 * 8);
        pieceBitboards[BLACK][ROOK] = 0x81ULL << (7 * 8);
        pieceBitboards[BLACK][QUEEN] = 0x08ULL << (7 * 8);
        pieceBitboards[BLACK][KING] = 0x10ULL << (7 * 8);

        updateOccupancies();
    }

    void Board::updateOccupancies()
    {
        occupancies[WHITE] = 0ULL;
        occupancies[BLACK] = 0ULL;

        for (int p = 0; p < PIECE_TYPE_COUNT; ++p)
        {
            occupancies[WHITE] |= pieceBitboards[WHITE][p];
            occupancies[BLACK] |= pieceBitboards[BLACK][p];
        }

        allPieces = occupancies[WHITE] | occupancies[BLACK];
    }

    void Board::printBoard()
    {
        std::cout << "\n  Standard Chess Board State:\n\n";
        for (int rank = 7; rank >= 0; rank--)
        {
            std::cout << (rank + 1) << "  ";
            for (int file = 0; file < 8; file++)
            {
                int square = rank * 8 + file;
                U64 bitMask = 1ULL << square;
                char pieceChar = '.';
                const char whiteChars[] = "PNBRQK";
                const char blackChars[] = "pnbrqk";

                for (int p = 0; p < PIECE_TYPE_COUNT; ++p)
                {
                    if (pieceBitboards[WHITE][p] & bitMask)
                        pieceChar = whiteChars[p];
                    if (pieceBitboards[BLACK][p] & bitMask)
                        pieceChar = blackChars[p];
                }
                std::cout << pieceChar << " ";
            }
            std::cout << "\n";
        }
        std::cout << "\n   a b c d e f g h\n\n";
    }

    bool Board::getPieceAt(int square, int &outColor, int &outPiece) const
    {
        U64 bitMask = 1ULL << square;
        if (!(allPieces & bitMask))
            return false;

        for (int c = 0; c < COLOR_COUNT; ++c)
        {
            if (occupancies[c] & bitMask)
            {
                for (int p = 0; p < PIECE_TYPE_COUNT; ++p)
                {
                    if (pieceBitboards[c][p] & bitMask)
                    {
                        outColor = c;
                        outPiece = p;
                        return true;
                    }
                }
            }
        }
        return false;
    }

    void Board::makeMove(Move move, int color)
    {
        int from = move.getFrom();
        int to = move.getTo();

        int movingPiece = -1;
        for (int p = 0; p < PIECE_TYPE_COUNT; ++p)
        {
            if (pieceBitboards[color][p] & (1ULL << from))
            {
                movingPiece = p;
                break;
            }
        }

        if (movingPiece == -1)
            return;

        pieceBitboards[color][movingPiece] &= ~(1ULL << from);
        pieceBitboards[color][movingPiece] |= (1ULL << to);

        int enemyColor = color ^ 1;
        for (int p = 0; p < PIECE_TYPE_COUNT; ++p)
        {
            if (pieceBitboards[enemyColor][p] & (1ULL << to))
            {
                pieceBitboards[enemyColor][p] &= ~(1ULL << to);
                break;
            }
        }

        if (movingPiece == PAWN && (to >= 56 || to <= 7))
        {
            pieceBitboards[color][PAWN] &= ~(1ULL << to);
            pieceBitboards[color][QUEEN] |= (1ULL << to);
        }

        updateOccupancies();
    }
}