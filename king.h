#ifndef KING_H
#define KING_H

#include "piece.h"

class King : public Piece {
public:
    King(Colour colour, Position pos);

    Piece* createPiece(int row, int col, Colour c) override;  
    std::vector<Position> getValidMoves(const Board &b) override;
};

#endif
