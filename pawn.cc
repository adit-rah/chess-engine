#include "pawn.h"
#include "board.h"


Pawn::Pawn(Colour colour, Position pos)
    : Piece(1, PieceType::Pawn, colour, pos) {} // value = 1 for pawn


std::vector<Position> Pawn::getRawMoves(const Board &b) {
    std::vector<Position> moves;
    Position current = getPosition();
    Colour myColour = getColour();

    int direction = (myColour == Colour::White) ? 1 : -1; // White moves up (row+1), Black moves down (row-1)
    int startRow = (myColour == Colour::White) ? 1 : 6;

    // 1. one square forward if empty
    Position oneAhead(current.row + direction, current.col);
    if (oneAhead.row >= 0 && oneAhead.row < 8) {
        Piece* oneAheadPiece = b.getPieceAt(oneAhead);
        if (oneAheadPiece == nullptr || oneAheadPiece->getType() == PieceType::None) {
            moves.push_back(oneAhead);

            // 2. two squares forward if first move and both empty
            Position twoAhead(current.row + 2 * direction, current.col);
            if (current.row == startRow) {
                Piece* twoAheadPiece = b.getPieceAt(twoAhead);
                if ((twoAhead.row >= 0 && twoAhead.row < 8) &&
                    (twoAheadPiece == nullptr || twoAheadPiece->getType() == PieceType::None)) {
                    moves.push_back(twoAhead);
                }
            }
        }
    }

    // 3. diagonal captures (left and right)
    int diagCols[] = {current.col - 1, current.col + 1};
    for (int c : diagCols) {
        Position diagPos(current.row + direction, c);
        if (diagPos.row >= 0 && diagPos.row < 8 && c >= 0 && c < 8) {
            Piece* target = b.getPieceAt(diagPos);
            if (target != nullptr && target->getType() != PieceType::None && target->getColour() != myColour) {
                moves.push_back(diagPos);
            }
        }
    }

    return moves;
}

// For getValidMoves we need to add en passant, and promotion.
std::vector<Position> Pawn::getValidMoves(Board &b) {
    std::vector<Position> rawMoves = getRawMoves(b);
    Position current = getPosition();
    std::vector<Position> moves;

    for (const Position& move : rawMoves) {
        if (b.isBoardLegalMove(current, move)) {
            moves.push_back(move);
        }
    }
    return moves;
}
