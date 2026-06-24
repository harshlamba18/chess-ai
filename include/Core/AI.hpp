#pragma once
#include "Core/Board.hpp"
#include "Core/Move.hpp"

namespace Engine {
    class AI {
    public:
        static Move getBestMove(const Board& board, int depth, int color);
    private:
        static int minimax(Board board, int depth, int alpha, int beta, int color);
    };
}