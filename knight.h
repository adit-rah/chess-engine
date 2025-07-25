#ifndef KNIGHT_H
#define KNIGHT_H

#include "piece.h"

// the subclasses aren't that special, they just get raw moves in a different way
class Knight : public Piece {
public:
    Knight(Colour colour, Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
};

#endif
