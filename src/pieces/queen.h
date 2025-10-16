#ifndef QUEEN_H
#define QUEEN_H

#include "piece.h"

// the subclasses aren't that special, they just get raw moves in a different way
class Queen : public Piece {
public:
    Queen(Colour colour, Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
};

#endif
