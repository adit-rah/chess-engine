#ifndef KING_H
#define KING_H
#include "piece.h"
class King : public Piece {
    
    public:
    King( Piece& Pawn);
    std::vector<Position> getRawMoves()
}

#endif