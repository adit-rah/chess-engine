#ifndef KING_H
#define KING_H

#include "piece.h"

// the subclasses aren't that special, they just get raw moves in a different way
// the king does have a bit more logic for castling (why we have to override getValidMoves as well)
class King : public Piece {
public:
    King(Colour colour, Position pos);

    std::vector<Position> getRawMoves(const Board &b) override;
    std::vector<Position> getValidMoves(Board &b) override;
};

#endif
