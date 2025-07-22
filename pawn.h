#ifndef PAWN_H
#define PAWN_H

#include "piece.h"

class Pawn : public Piece {
public:
    Pawn(Colour colour, Position pos);

    Piece* createPiece(int row, int col, Colour c) override;
    std::vector<Position> getValidMoves(const Board &b) override;
};

#endif
