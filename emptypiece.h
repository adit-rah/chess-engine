#ifndef EMPTYPIECE_H
#define EMPTYPIECE_H

#include "piece.h"

class EmptyPiece : public Piece {
public:
    EmptyPiece(Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
    // std::vector<Position> getValidMoves(Board &b) override;
};

#endif
