#ifndef PAWN_H
#define PAWN_H

#include "piece.h"

// the subclasses aren't that special, they just get raw moves in a different way
// pawn accounts for enpassent
class Pawn : public Piece {
public:
    Pawn(Colour colour, Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
    std::vector<Position> getValidMoves(Board &b) override;
};

#endif
