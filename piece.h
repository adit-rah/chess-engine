#ifndef PIECE_H
#define PIECE_H
#include <iostream>
#include <vector>
#include "board.h"

enum class Colour {White, Black, None};
enum class PieceType {None, Pawn, Bishop, Knight, Rook, Queen, King};

class Position {
    int row;
    int col;
    public:
        bool operator==(const Position& other);
};

class Piece {
    int value;
    PieceType type;
    Colour colour;
    Position pos;

    public:
        virtual ~Piece() {}
        Piece(int value, PieceType type, Colour colour, Position pos);
        Colour getColour();
        Position getPosition();
        void setPosition(Position p);
        char getSymbol();
        PieceType getType() { return type; }
        Piece* createPiece(int row, int col);
        virtual std::vector<Position> getRawMoves() = 0; //abstract method
        virtual std::vector<Position> getValidMoves() = 0; //abstract method

};

#endif