#include "knight.h"
#include "board.h"


Knight::Knight(Colour colour, Position pos)
    : Piece(3, PieceType::Knight, colour, pos) {} // value = 3 for knight


std::vector<Position> Knight::getRawMoves(const Board &b) {
    std::vector<Position> moves;
    Position current = getPosition();
    Colour myColour = getColour();

    // Knight moves: 8 possible "L" shapes
    int moveRow[] = {-2, -2, -1, -1, 1, 1, 2, 2};
    int moveCol[] = {-1, 1, -2, 2, -2, 2, -1, 1};

    for (int i = 0; i < b.getBoardSize(); ++i) {
        int r = current.row + moveRow[i];
        int c = current.col + moveCol[i];

        if (r >= 0 && r < b.getBoardSize() && c >= 0 && c < b.getBoardSize()) {
            Piece *target = b.getPieceAt(Position(r, c));
            if (!target || target->getType() == PieceType::None || target->getColour() != myColour) {
                moves.push_back(Position(r, c));
            }
        }
    }
    return moves;
}
