#include "rook.h"
#include "board.h"


Rook::Rook(Colour colour, Position pos)
    : Piece(5, PieceType::Rook, colour, pos) {} // value = 5 for rook


// still does not check for king safety and other such rules
// but it prunes for blocked paths for effeciencies sake
std::vector<Position> Rook::getRawMoves(const Board &b) {
    std::vector<Position> moves;
    Position current = getPosition();
    Colour myColour = getColour();

    // Directions: up, down, left, right
    int dirRow[] = {-1, 1, 0, 0};
    int dirCol[] = {0, 0, -1, 1};

    for (int d = 0; d < 4; d++) {               // check in all four directions
        int r = current.row + dirRow[d];
        int c = current.col + dirCol[d];

        // Move along this direction until blocked
        while (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
            Position nextPos(r, c);
            Piece *target = b.getPieceAt(nextPos);

            if (!target || target->getType() == PieceType::None) {
                // Empty square --> valid move
                moves.push_back(nextPos);
            } else {
                // Occupied square --> can capture if enemy
                if (target->getColour() != myColour) {
                    moves.push_back(nextPos);
                }
                // Stop exploring this direction (blocked)
                break;
            }

            r += dirRow[d];
            c += dirCol[d];
        }
    }

    return moves;
}
