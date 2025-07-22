#ifndef BOARD_H
#define BOARD_H

#include "subject.h"
#include "piece.h"

class Board : public Subject {
    Piece ***pieces;    // "triple pointer AHH", no it's not that bad.
                        // It's just a 2-D array (chess board; the first two *) of Piece pointers

public:
    Board();
    ~Board();
    
    // board set up functions
    void setPieces();
    void resetBoard();

    // move validation
    Piece* getPieceAt(Position p) const;
    bool validMove(Position frome, Position to); 
    bool movePiece(Position from, Position to); 
    bool canMove(Piece& p) const;

    // attacking logic and handling
    std::vector<Position> squaresBeingAttackedBy(Colour c) const;
    bool isInCheck(Colour c) const;
    bool isCheckMate(Colour c) const;
    Position findKing(Colour c) const;
};

#endif
