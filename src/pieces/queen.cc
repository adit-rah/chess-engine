#include "queen.h"
#include "board.h"


Queen::Queen(Colour colour, Position pos)
    : Piece(9, PieceType::Queen, colour, pos) {} // value = 9 for queen


std::vector<Position> Queen::getRawMoves(const Board &b) {
    std::vector<Position> moves;
    Position current = getPosition();
    Colour myColour = getColour();

    // Directions: rook + bishop combined
    int dirRow[] = {-1, 1, 0, 0, -1, -1, 1, 1};
    int dirCol[] = {0, 0, -1, 1, -1, 1, -1, 1};

    for (int d = 0; d < b.getBoardSize(); ++d) {
        int r = current.row + dirRow[d];
        int c = current.col + dirCol[d];

        while (r >= 0 && r < b.getBoardSize() && c >= 0 && c < b.getBoardSize()) {
            Piece *target = b.getPieceAt(Position(r, c));

            if (!target || target->getType() == PieceType::None) {
                moves.push_back(Position(r, c));
            } else {
                if (target->getColour() != myColour) {
                    moves.push_back(Position(r, c));
                }
                break; // blocked
            }
            r += dirRow[d];
            c += dirCol[d];
        }
    }
    return moves;
}
