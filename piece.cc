#include "piece.h"


bool Position::operator==(const Position& other){
   return row == other.row && col == other.col;
}

Piece::Piece(int value, PieceType type, Colour colour, Position pos) :
    value{value}, type{type}, colour{colour}, pos{pos} {}

Colour Piece::getColour(){
    return this.colour;
}

Position Piece::getPosition(){
    return this.pos;
}

void Piece::setPosition(Position p){
    pos = p;
}

char Piece::getSymbol(){
    if (type == PieceType::King) return 'K';
    else if (type == PieceType::Queen) return 'Q';
    else if (type == PieceType::Rook) return 'R';
    else if (type == PieceType::Knight) return 'N';
    else if (type == PieceType::Bishop) return 'B';
    else if (type == PieceType::Pawn) return 'P';
    else if (type == PieceType::None) return '_';
}


Piece* createPiece(int row, int col) {
    Colour c;
    if (row < 2) c = Colour::White;
    else if (row > 5) c = Colour::Black;
    else c = Colour::None;

    Position pos{row, col};

    if (row == 1 || row == 6) return new Pawn(c, pos);
    if (row == 0 || row == 7) {
        if (col == 0 || col == 7) return new Rook(c, pos);
        if (col == 1 || col == 6) return new Knight(c, pos);
        if (col == 2 || col == 5) return new Bishop(c, pos);
        if (col == 3) return new Queen(c, pos);
        if (col == 4) return new King(c, pos);
    }
    return Pawn(Color::None, pos); // Default case for empty squares
}

// add getValid moves later
