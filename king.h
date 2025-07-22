#ifndef KING_H
#define KING_H

#include "piece.h"

class King : public Piece {
public:
    King(Colour colour, Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
    std::vector<Position> getValidMoves(const Board &b) override;
};

#endif
