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
    
    // You will notice that some methods that should be const aren't. The reason why
    // this is the case is because we do some fast pin detection that alters the 
    // state of the board, making it not const.
    
    // board set up functions =========
    
    // sets pieces in the default way
    void setPieces();
    // resets all the pieces to default positions
    void resetBoard();

    // move validation =================
    
    // returns the piece at that position
    Piece* getPieceAt(Position p) const;
    // checks if a move is board-legal: i.e., it doesn’t leave the moving side’s king in check.
    // does NOT verify the piece’s own movement rules—only overall board legality.
    bool isBoardLegalMove(Position from, Position to);
    // moves the piece, returns false if invalid move
    bool movePiece(Position from, Position to); 
    // handles the board's special moves (castling, enpassent, promotions in this case)
    bool handleSpecialMoves(Position from, Position to);
    // checks if a piece can move 
    bool canMove(Piece& p);

    // placement validation ============
    
    // places a piece based of the symbol at that position 
    void placePiece(char pieceSymbol, Position pos);
    // removes the piece at that position
    void removePiece(Position pos);
    // validates setup in accordance to setup mode requirements
    bool validateSetup() const; 

    // attacking logic and handling ====
    
    // returns all squares being attacked by the colour c
    std::vector<Position> squaresBeingAttackedBy(Colour c) const;
    // is the king in the line of sight of any enemy piece?
    bool isInCheck(Colour c) const;
    // are there any moves left?
    bool isCheckMate(Colour c);
    // finds the kings position on the board
    Position findKing(Colour c) const;
};

#endif
