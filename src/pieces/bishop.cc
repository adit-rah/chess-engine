#include "bishop.h"
#include "board.h"


Bishop::Bishop(Colour colour, Position pos)
    : Piece(3, PieceType::Bishop, colour, pos) {} // value = 3 for bishop


std::vector<Position> Bishop::getRawMoves(const Board &b) {
    std::vector<Position> moves;
    Position current = getPosition();
    Colour myColour = getColour();

    // Directions: diagonal (up-left, up-right, down-left, down-right)
    int dirRow[] = {-1, -1, 1, 1};
    int dirCol[] = {-1, 1, -1, 1};

    for (int d = 0; d < 4; ++d) {
        int r = current.row + dirRow[d];
        int c = current.col + dirCol[d];

        while (r >= 0 && r < b.getBoardSize() && c >= 0 && c < b.getBoardSize()) {
            Piece *target = b.getPieceAt(Position(r, c));

            if (!target || target->getType() == PieceType::None) {
                moves.push_back(Position(r, c));        // empty so we can move through it
            } else {
                if (target->getColour() != myColour) {  // we can capture that piece
                    moves.push_back(Position(r, c));
                }
                break; // blocked otherwise
            }
            r += dirRow[d];
            c += dirCol[d];
        }
    }
    return moves;
}
