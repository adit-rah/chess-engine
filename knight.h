#ifndef KNIGHT_H
#define KNIGHT_H

#include "piece.h"

class Knight : public Piece {
public:
    Knight(Colour colour, Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
    // std::vector<Position> getValidMoves(Board &b) override;
};

#endif
