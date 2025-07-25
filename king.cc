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

    // normal adjacent squares
    for (int i = 0; i < BOARD_SIZE; ++i) {
        int r = current.row + dirRow[i];
        int c = current.col + dirCol[i];

        if (r >= 0 && r < BOARD_SIZE && c >= 0 && c < BOARD_SIZE) {
            Piece* target = b.getPieceAt(Position(r, c));
            if (target == nullptr || target->getColour() != myColour) {
                moves.push_back(Position(r, c));
            }
        }
    }

    // castling will be handled elsewhere

    return moves;
}


std::vector<Position> King::getValidMoves(Board &b) {
    std::vector<Position> rawMoves = getRawMoves(b);
    Position current = getPosition();
    Colour myColour = getColour();
    std::vector<Position> moves; 

    for (Position &move: rawMoves) {
        if (b.isBoardLegalMove(current, move)) {
            moves.push_back(move); 
        }
    }

    // this is where we handle castling since it's a special move
    if (!getHasMoved()) {
        Colour opp = (myColour == Colour::White) ? Colour::Black : Colour::White;

        // lambda for speed :O
        std::vector<Position> attacked = b.squaresBeingAttackedBy(opp);
        auto isAttacked = [&](Position p) -> bool {     // same logic as isSquareBeingAttacked
            for (Position &sq : attacked) {             // but i just didn't want to keep
                if (sq == p) return true;               // recalculating over the board
            }
            return false;
        };
        
        // lambda for clarity :O
        auto isEmpty = [&](Position p) {
            Piece* target = b.getPieceAt(p);
            return (target == nullptr || target->getType() == PieceType::None);
        };

        int rank = (myColour == Colour::White ? 0 : 7);

        // Kingside Castling
        Position rookKs(rank, 7);               // h1 or h8 (we will assume h1 for naming)
        Piece *ksRook = b.getPieceAt(rookKs);
        if (ksRook && ksRook->getType() == PieceType::Rook && !ksRook->getHasMoved()) {
            Position f1(rank, 5);
            Position g1(rank, 6);

            // check if path is empty + safe
            if (isEmpty(f1) && isEmpty(g1) &&
                !isAttacked(current) && !isAttacked(f1) && !isAttacked(g1)) {
                moves.push_back(g1);
            }
        }

        // Queenside Castling
        Position rookQs(rank, 0);               // a1 or a8 (again assume a1 for naming)
        Piece *qsRook = b.getPieceAt(rookQs);
        if (qsRook && qsRook->getType() == PieceType::Rook && !qsRook->getHasMoved()) {
            Position d1(rank, 3);
            Position c1(rank, 2);
            Position b1(rank, 1);

            // same thing
            if (isEmpty(b1) && isEmpty(c1) && isEmpty(d1) &&
                !isAttacked(current) && !isAttacked(d1) && !isAttacked(c1)) {
                moves.push_back(c1);
            }
        }
    }
    
    return moves;
}

