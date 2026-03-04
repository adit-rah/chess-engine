#ifndef ROOK_H
#define ROOK_H

#include "piece.h"

// the subclasses aren't that special, they just get raw moves in a different way
class Rook : public Piece {
public:
    Rook(Colour colour, Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
};

#endif
