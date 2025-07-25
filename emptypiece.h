#ifndef EMPTYPIECE_H
#define EMPTYPIECE_H

#include "piece.h"

// the subclasses aren't that special, they just get raw moves in a different way
class EmptyPiece : public Piece {
public:
    EmptyPiece(Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
};

#endif
