#ifndef PIECE_H
#define PIECE_H

#include <vector>

class Board; 

enum class Colour {White, Black, None};
enum class PieceType {None, Pawn, Bishop, Knight, Rook, Queen, King};

class Position {
    public:
        int row;
        int col;
        Position(int r = 0, int c = 0); 
        bool operator==(const Position& other) const;
};

class Piece {
    int value;
    PieceType type;
    Colour colour;
    Position pos;
    bool hasMoved = false;

    public:
        Piece();
        Piece(Position pos);
        Piece(int value, PieceType type, Colour colour, Position pos);
        Piece& operator=(const Piece& other);
        virtual ~Piece() {}
        int getValue() const;
        Colour getColour();
        Position getPosition();
        void setPosition(Position p);
        char getSymbol();
        PieceType getType();
        void setHasMoved(bool moved) { hasMoved = moved; } // i should probably move these to the
        bool getHasMoved() const { return hasMoved; }       // implementation file
        
        // abstract method(s)
        // slightly misleading since it still prunes moves out based on "visibility"
        virtual std::vector<Position> getRawMoves(const Board &b) = 0;
        // gets game legal moves
        virtual std::vector<Position> getValidMoves(Board &b);

};

#endif
