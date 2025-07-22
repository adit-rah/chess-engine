#include "emptypiece.h"
#include "board.h"


EmptyPiece::EmptyPiece(Position pos)
    : Piece(0, PieceType::None, Colour::None, pos) {}


Piece* EmptyPiece::createPiece(int row, int col, Colour c) {    // Colour shouldn't be taken
    Position pos{row, col};                                         
    return new EmptyPiece(pos); // Default case for empty squares
}


std::vector<Position> EmptyPiece::getValidMoves(const Board &b) {
    // blank piece DOES NOT have moves by default
}