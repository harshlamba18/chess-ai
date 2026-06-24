#include "Core/MoveGenerator.hpp"
#include "Core/BitOps.hpp"

namespace Engine {

    std::vector<Move> MoveGenerator::generatePseudoLegalMoves(const Board& board, int color) {
        std::vector<Move> moveList;
        moveList.reserve(256);

        generatePawnMoves(board, color, moveList);
        generateKnightMoves(board, color, moveList);
        generateKingMoves(board, color, moveList);
        generateSlidingMoves(board, color, moveList);

        return moveList;
    }

    void MoveGenerator::generatePawnMoves(const Board& board, int color, std::vector<Move>& moveList) {
        U64 pawns = board.pieceBitboards[color][PAWN];
        U64 emptySquares = ~board.allPieces;

        if (color == WHITE) {
            U64 singlePushes = (pawns << 8) & emptySquares;
            U64 pushBoard = singlePushes;
            
            while (pushBoard) {
                int to = popLSB(pushBoard);
                moveList.push_back(Move(to - 8, to, 0));
            }

            U64 doublePushes = ((singlePushes & 0x0000000000FF0000ULL) << 8) & emptySquares;
            U64 doubleBoard = doublePushes;
            
            while (doubleBoard) {
                int to = popLSB(doubleBoard);
                moveList.push_back(Move(to - 16, to, 0));
            }

            U64 enemies = board.occupancies[BLACK];
            U64 capturesNW = (pawns << 7) & notHFile & enemies;
            while (capturesNW) {
                int to = popLSB(capturesNW);
                moveList.push_back(Move(to - 7, to, 0));
            }

            U64 capturesNE = (pawns << 9) & notAFile & enemies;
            while (capturesNE) {
                int to = popLSB(capturesNE);
                moveList.push_back(Move(to - 9, to, 0));
            }

        } else {
            U64 singlePushes = (pawns >> 8) & emptySquares;
            U64 pushBoard = singlePushes;
            
            while (pushBoard) {
                int to = popLSB(pushBoard);
                moveList.push_back(Move(to + 8, to, 0));
            }

            U64 doublePushes = ((singlePushes & 0x0000FF0000000000ULL) >> 8) & emptySquares;
            U64 doubleBoard = doublePushes;
            
            while (doubleBoard) {
                int to = popLSB(doubleBoard);
                moveList.push_back(Move(to + 16, to, 0));
            }

            U64 enemies = board.occupancies[WHITE];
            U64 capturesSE = (pawns >> 7) & notAFile & enemies;
            while (capturesSE) {
                int to = popLSB(capturesSE);
                moveList.push_back(Move(to + 7, to, 0));
            }

            U64 capturesSW = (pawns >> 9) & notHFile & enemies;
            while (capturesSW) {
                int to = popLSB(capturesSW);
                moveList.push_back(Move(to + 9, to, 0));
            }
        }
    }

    void MoveGenerator::generateKnightMoves(const Board& board, int color, std::vector<Move>& moveList) {
        U64 knights = board.pieceBitboards[color][KNIGHT];
        U64 validSquares = ~board.occupancies[color];

        while (knights) {
            int from = popLSB(knights);
            U64 k = 1ULL << from;
            
            U64 attacks = ((k << 17) & notAFile) |
                          ((k << 15) & notHFile) |
                          ((k << 10) & notABFile) |
                          ((k << 6)  & notGHFile) |
                          ((k >> 15) & notAFile) |
                          ((k >> 17) & notHFile) |
                          ((k >> 6)  & notABFile) |
                          ((k >> 10) & notGHFile);

            U64 validAttacks = attacks & validSquares;
            
            while (validAttacks) {
                int to = popLSB(validAttacks);
                moveList.push_back(Move(from, to, 0));
            }
        }
    }

    void MoveGenerator::generateKingMoves(const Board& board, int color, std::vector<Move>& moveList) {
        U64 king = board.pieceBitboards[color][KING];
        U64 validSquares = ~board.occupancies[color];

        while (king) {
            int from = popLSB(king);
            U64 k = 1ULL << from;

            U64 attacks = ((k << 8)) |
                          ((k >> 8)) |
                          ((k << 1) & notAFile) |
                          ((k >> 1) & notHFile) |
                          ((k << 9) & notAFile) |
                          ((k << 7) & notHFile) |
                          ((k >> 7) & notAFile) |
                          ((k >> 9) & notHFile);

            U64 validAttacks = attacks & validSquares;

            while (validAttacks) {
                int to = popLSB(validAttacks);
                moveList.push_back(Move(from, to, 0));
            }
        }
    }

    void MoveGenerator::generateSlidingMoves(const Board& board, int color, std::vector<Move>& moveList) {
        U64 rooks = board.pieceBitboards[color][ROOK];
        U64 bishops = board.pieceBitboards[color][BISHOP];
        U64 queens = board.pieceBitboards[color][QUEEN];
        U64 friendly = board.occupancies[color];
        U64 all = board.allPieces;

        auto castRays = [&](U64 pieces, const std::vector<int>& directions) {
            U64 piecesCopy = pieces;
            while (piecesCopy) {
                int from = popLSB(piecesCopy);
                for (int dir : directions) {
                    int to = from;
                    while (true) {
                        bool hitEdge = false;
                        if ((dir == 1 || dir == 9 || dir == -7) && (to % 8 == 7)) hitEdge = true;
                        if ((dir == -1 || dir == -9 || dir == 7) && (to % 8 == 0)) hitEdge = true;
                        if (hitEdge) break;

                        to += dir;
                        if (to < 0 || to > 63) break;

                        U64 toMask = 1ULL << to;
                        if (friendly & toMask) break; 
                        
                        moveList.push_back(Move(from, to, 0));
                        
                        if (all & toMask) break; 
                    }
                }
            }
        };

        castRays(rooks | queens, {8, -8, -1, 1});
        castRays(bishops | queens, {7, -7, 9, -9});
    }
    bool MoveGenerator::isSquareAttacked(const Board& board, int square, int attackerColor) {
        U64 sqMask = 1ULL << square;
        
        if (attackerColor == WHITE) {
            if (((sqMask >> 7) & notAFile) & board.pieceBitboards[WHITE][PAWN]) return true;
            if (((sqMask >> 9) & notHFile) & board.pieceBitboards[WHITE][PAWN]) return true;
        } else {
            if (((sqMask << 7) & notHFile) & board.pieceBitboards[BLACK][PAWN]) return true;
            if (((sqMask << 9) & notAFile) & board.pieceBitboards[BLACK][PAWN]) return true;
        }

        U64 k = sqMask;
        U64 knightAttacks = ((k << 17) & notAFile) | ((k << 15) & notHFile) |
                            ((k << 10) & notABFile) | ((k << 6)  & notGHFile) |
                            ((k >> 15) & notAFile) | ((k >> 17) & notHFile) |
                            ((k >> 6)  & notABFile) | ((k >> 10) & notGHFile);
        if (knightAttacks & board.pieceBitboards[attackerColor][KNIGHT]) return true;

        U64 kingAttacks = ((k << 8)) | ((k >> 8)) |
                          ((k << 1) & notAFile) | ((k >> 1) & notHFile) |
                          ((k << 9) & notAFile) | ((k << 7) & notHFile) |
                          ((k >> 7) & notAFile) | ((k >> 9) & notHFile);
        if (kingAttacks & board.pieceBitboards[attackerColor][KING]) return true;

        int orthoDirs[4] = {8, -8, -1, 1};
        for (int dir : orthoDirs) {
            int to = square;
            while (true) {
                bool hitEdge = false;
                if ((dir == 1) && (to % 8 == 7)) hitEdge = true;
                if ((dir == -1) && (to % 8 == 0)) hitEdge = true;
                if (hitEdge) break;

                to += dir;
                if (to < 0 || to > 63) break;

                U64 toMask = 1ULL << to;
                if (board.pieceBitboards[attackerColor][ROOK] & toMask || 
                    board.pieceBitboards[attackerColor][QUEEN] & toMask) return true;
                
                if (board.allPieces & toMask) break; 
            }
        }

        int diagDirs[4] = {7, -7, 9, -9};
        for (int dir : diagDirs) {
            int to = square;
            while (true) {
                bool hitEdge = false;
                if ((dir == 9 || dir == -7) && (to % 8 == 7)) hitEdge = true;
                if ((dir == -9 || dir == 7) && (to % 8 == 0)) hitEdge = true;
                if (hitEdge) break;

                to += dir;
                if (to < 0 || to > 63) break;

                U64 toMask = 1ULL << to;
                if (board.pieceBitboards[attackerColor][BISHOP] & toMask || 
                    board.pieceBitboards[attackerColor][QUEEN] & toMask) return true;
                
                if (board.allPieces & toMask) break; 
            }
        }

        return false;
    }

    bool MoveGenerator::isInCheck(const Board& board, int color) {
        int kingSquare = getLSB(board.pieceBitboards[color][KING]);
        return isSquareAttacked(board, kingSquare, color ^ 1);
    }

    std::vector<Move> MoveGenerator::generateLegalMoves(const Board& board, int color) {
        std::vector<Move> pseudo = generatePseudoLegalMoves(board, color);
        std::vector<Move> legal;
        legal.reserve(pseudo.size());

        for (const Move& move : pseudo) {
            Board tempBoard = board;
            tempBoard.makeMove(move, color);
            
            if (!isInCheck(tempBoard, color)) {
                legal.push_back(move);
            }
        }
        return legal;
    }
}
