#ifndef PIECE_H
#define PIECE_H
#include <iostream>
#include <vector>
#include "board.h"

enum class Colour {White, Black, None};
enum class PieceType {None, Pawn, Bishop, Knight, Rook, Queen, King};

class Position {
    public:
        int row;
        int col;
        Position(int r = 0, int c = 0); 
        bool operator==(const Position& other);
};

class Piece {
    int value;
    PieceType type;
    Colour colour;
    Position pos;

    public:
        Piece();
        Piece(int value, PieceType type, Colour colour, Position pos);
        Piece& operator=(const Piece& other);
        virtual ~Piece() {}

        Colour getColour();
        Position getPosition();
        void setPosition(Position p);
        char getSymbol();
        PieceType getType();
        
        virtual Piece* createPiece(int row, int col, Colour c);
        virtual std::vector<Position> getValidMoves(const Board &b);

};

#endif
