#include "emptypiece.h"
#include "board.h"


EmptyPiece::EmptyPiece(Position pos)
    : Piece(0, PieceType::None, Colour::None, pos) {}
    

std::vector<Position> EmptyPiece::getRawMoves(const Board &b) {
    // blank piece DOES NOT have moves by default
    std::vector<Position> moves; // empty vector
    return moves;
}
