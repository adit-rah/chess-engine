#ifndef EMPTYPIECE_H
#define EMPTYPIECE_H

#include "piece.h"

class EmptyPiece : public Piece {
public:
    EmptyPiece(Position pos);

    Piece* createPiece(int row, int col, Colour c) override;  
    std::vector<Position> getRawMoves(const Board &b) override;
    std::vector<Position> getValidMoves(const Board &b) override;
};

#endif
