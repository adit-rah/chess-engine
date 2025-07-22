#ifndef PAWN_H
#define PAWN_H

#include "piece.h"

class Pawn : public Piece {
public:
    Pawn(Colour colour, Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
    std::vector<Position> getValidMoves(const Board &b) override;
};

#endif
