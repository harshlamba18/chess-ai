#pragma once
#include <cstdint>

namespace Engine {
    class Move {
    public:
        uint16_t data;

        Move();
        Move(int from, int to, int flags = 0);

        int getFrom() const;
        int getTo() const;
        int getFlags() const;
    };
}