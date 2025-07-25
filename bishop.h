#ifndef BISHOP_H
#define BISHOP_H

#include "piece.h"

// the subclasses aren't that special, they just get raw moves in a different way
class Bishop : public Piece {
public:
    Bishop(Colour colour, Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
};

#endif
