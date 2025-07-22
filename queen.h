#ifndef QUEEN_H
#define QUEEN_H

#include "piece.h"

class Queen : public Piece {
public:
    Queen(Colour colour, Position pos);

    Piece* createPiece(int row, int col, Colour c) override;  
    std::vector<Position> getValidMoves(const Board &b) override;
};

#endif
