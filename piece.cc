#include "piece.h"
#include "board.h"

// Position's implementations
Position::Position(int r, int c) : row{r}, col{c} {}


bool Position::operator==(const Position& other) const{
   return row == other.row && col == other.col;
}

// Piece's implementations
Piece::Piece() :
    value{0}, type{PieceType::None}, colour{Colour::None}, pos{0,0} {}  // default

Piece::Piece(Position pos) :
    value{0}, type{PieceType::None}, colour{Colour::None}, pos{pos} {}  // default

Piece::Piece(int value, PieceType type, Colour colour, Position pos) :
    value{value}, type{type}, colour{colour}, pos{pos} {}


Piece& Piece::operator=(const Piece& other) {
    if (this == &other) return *this;

    this->value = other.value;
    this->type = other.type;
    this->colour = other.colour;
    this->pos = other.pos;

    return *this;
}


Colour Piece::getColour() {
    return colour;
}


PieceType Piece::getType() { 
    return type; 
}


Position Piece::getPosition() {
    return pos;
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
    return ' ';
}

// this isn't abstract since this tends to be (except for pawn and king) the case
std::vector<Position> Piece::getValidMoves(Board &b) {
    std::vector<Position> rawMoves = getRawMoves(b);
    Position current = getPosition();
    std::vector<Position> moves; 

    for (Position &move: rawMoves) {
        if (b.validMove(current, move)) {
            moves.push_back(move); 
        }
    }
    
    return moves;
}
