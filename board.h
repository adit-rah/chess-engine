#ifndef BOARD_H
#define BOARD_H

#include "subject.h";
#include "piece.h";  

class Board : public Subject {
    Piece ***pieces;    // "triple pointer AHH", no it's not that bad.
                        // It's just a 2-D array (chess board; the first two *) of Piece pointers

    public:
    Board();
    ~Board();

    void setPieces();
    void resetBoard();

    Piece* getPieceAt(Position p) const;
    bool movePiece(Position from, Position to); // this should not fail, but still better for error handling
    bool validMove(Position frome, Position to) const;  // added this one  
    bool canMove(Piece& p) const;

    std::vector<Position> squaresBeingAttacked(Colour c) const;
    bool isCheckMate(Colour c) const;
    Position findKing(Colour c) const;                   // and this one
}

#endif
