#ifndef KNIGHT_H
#define KNIGHT_H

#include "piece.h"

class Knight : public Piece {
public:
    Knight(Colour colour, Position pos);

    Piece* createPiece(int row, int col, Colour c) override;  
    std::vector<Position> getValidMoves(const Board &b) override;
};

#endif
