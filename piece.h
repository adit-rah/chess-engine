#ifndef PIECE_H
#define PIECE_H
#include <iostream>
#include "board.h"

enum class Colour {White, Black, None};
enum class PieceType {None, Pawn, Bishop, Knight, Rook, Queen, King};

class Position {
    int row;
    int col;
    public:
        Position& operator=(const Position& other);
}

class Piece {
    int value;
    PieceType type;
    Colour colour;
    Position pos;

    public:
        Piece(int value, PieceType type, Colour colour, Position pos);
        Colour getColour();
        Position getPosition();
        void setPosition();
        char getSymbol();
        std::vector<Position> getValidMoves();

}

#endif