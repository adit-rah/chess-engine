#ifndef BISHOP_H
#define BISHOP_H

#include "piece.h"

class Bishop : public Piece {
public:
    Bishop(Colour colour, Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
    // std::vector<Position> getValidMoves(Board &b) override;
};

#endif
