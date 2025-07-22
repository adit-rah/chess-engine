#include "king.h"
#include "board.h"


King::King(Colour colour, Position pos)
    : Piece(1e5, PieceType::King, colour, pos) {} // value is 1 bajillion to signify importance


std::vector<Position> King::getRawMoves(const Board &b) {
    std::vector<Position> moves;
    Position current = getPosition();
    Colour myColour = getColour();

    int dirRow[] = {-1, -1, -1, 0, 0, 1, 1, 1};
    int dirCol[] = {-1, 0, 1, -1, 1, -1, 0, 1};

    // 1. Normal adjacent squares
    for (int i = 0; i < 8; ++i) {
        int r = current.row + dirRow[i];
        int c = current.col + dirCol[i];

        if (r >= 0 && r < 8 && c >= 0 && c < 8) {
            Piece* target = b.getPieceAt(Position(r, c));
            if (target == nullptr || target->getColour() != myColour) {
                moves.push_back(Position(r, c));
            }
        }
    }

    // 2. Castling moves (kingside and queenside)
    // still need to do it
    
    return moves;
}
