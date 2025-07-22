#ifndef BOARD_H
#define BOARD_H

#include "subject.h";
#include "piece.h";  

class Board : public subject {
    Piece **pieces;

    public:
    Board();
    virtual void setPieces();

    Piece* getPieceAt(Position p) const;
    bool movePiece(Position from, Position to);
    bool validMove(Position frome, Position to) const;  // added this one  
    bool canMove(Piece& p) const;

    std::vector<Position> squaresBeingAttacked(Color c) const;
    bool isCheckMate(Color c) const;
    Position findKing(Color c) const;                   // and this one

    void resetBoard();
}

#endif