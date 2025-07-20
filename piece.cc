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

// add getValid moves later
