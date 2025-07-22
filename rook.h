#ifndef ROOK_H
#define ROOK_H

#include "piece.h"

class Rook : public Piece {
public:
    Rook(Colour colour, Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
    // std::vector<Position> getValidMoves(Board &b) override;
};

#endif
