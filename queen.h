#ifndef QUEEN_H
#define QUEEN_H

#include "piece.h"

class Queen : public Piece {
public:
    Queen(Colour colour, Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
    // std::vector<Position> getValidMoves(Board &b) override;
};

#endif
