#pragma once
#include "Core/Board.hpp"

namespace Engine {
    class Evaluation {
    public:
        static int evaluate(const Board& board);
    };
}