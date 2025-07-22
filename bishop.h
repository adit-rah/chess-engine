#ifndef BISHOP_H
#define BISHOP_H

#include "piece.h"

class Bishop : public Piece {
public:
    Bishop(Colour colour, Position pos);

    Piece* createPiece(int row, int col, Colour c) override;  
    std::vector<Position> getValidMoves(const Board &b) override;
};

#endif
