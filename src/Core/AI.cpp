#include "Core/AI.hpp"
#include "Core/Evaluation.hpp"
#include "Core/MoveGenerator.hpp"
#include <algorithm>
#include <vector>

namespace Engine {
    Move AI::getBestMove(const Board& board, int depth, int color) {
        std::vector<Move> legalMoves = MoveGenerator::generateLegalMoves(board, color);
        Move bestMove;
        int bestScore = (color == WHITE) ? -9999999 : 9999999;

        for (const Move& move : legalMoves) {
            Board tempBoard = board;
            tempBoard.makeMove(move, color);
            int score = minimax(tempBoard, depth - 1, -9999999, 9999999, color ^ 1);

            if (color == WHITE) {
                if (score > bestScore) {
                    bestScore = score;
                    bestMove = move;
                }
            } else {
                if (score < bestScore) {
                    bestScore = score;
                    bestMove = move;
                }
            }
        }
        return bestMove;
    }

    int AI::minimax(Board board, int depth, int alpha, int beta, int color) {
        if (depth == 0) {
            return Evaluation::evaluate(board);
        }

        std::vector<Move> legalMoves = MoveGenerator::generateLegalMoves(board, color);
        
        if (legalMoves.empty()) {
            if (MoveGenerator::isInCheck(board, color)) {
                return (color == WHITE) ? -9999999 : 9999999;
            }
            return 0;
        }

        if (color == WHITE) {
            int maxScore = -9999999;
            for (const Move& move : legalMoves) {
                Board tempBoard = board;
                tempBoard.makeMove(move, color);
                int score = minimax(tempBoard, depth - 1, alpha, beta, BLACK);
                maxScore = std::max(maxScore, score);
                alpha = std::max(alpha, score);
                if (beta <= alpha) break; 
            }
            return maxScore;
        } else {
            int minScore = 9999999;
            for (const Move& move : legalMoves) {
                Board tempBoard = board;
                tempBoard.makeMove(move, color);
                int score = minimax(tempBoard, depth - 1, alpha, beta, WHITE);
                minScore = std::min(minScore, score);
                beta = std::min(beta, score);
                if (beta <= alpha) break; 
            }
            return minScore;
        }
    }
}