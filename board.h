#ifndef BOARD_H
#define BOARD_H

class board : public subject {

    Piece ** pieces;

    public:
    Board();
    Piece* getPieceAt(Position p);
    bool movePiece(Position from, Position to);
    std::vector<Position> squaresBeingAttacked(Color c);
    bool isCheckMate(Color c);
    bool canMove(Piece& p);
    void resetBoard();
}

#endif