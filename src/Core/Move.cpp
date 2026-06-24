#include "Core/Move.hpp"

namespace Engine {
    Move::Move() : data(0) {}

    Move::Move(int from, int to, int flags) {
        data = (from & 0x3F) | ((to & 0x3F) << 6) | ((flags & 0xF) << 12);
    }

    int Move::getFrom() const {
        return data & 0x3F;
    }

    int Move::getTo() const {
        return (data >> 6) & 0x3F;
    }

    int Move::getFlags() const {
        return (data >> 12) & 0xF;
    }
}