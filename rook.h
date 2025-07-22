#ifndef ROOK_H
#define ROOK_H

#include "piece.h"

class Rook : public Piece {
public:
    Rook(Colour colour, Position pos);

    Piece* createPiece(int row, int col, Colour c) override;  
    std::vector<Position> getValidMoves(const Board &b) override;
};

#endif
