#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"

#include <sstream>

// abstract player class
class Player {
protected:
    Colour colour;
public:
    Player(Colour c) : colour{c} {}
    virtual ~Player() = default;

    Colour getColour() const { return colour; }

    // at their heart is their specifc make move function that defines how they make moves
    virtual bool makeMove(Board &board, std::istringstream &iss) = 0;
};

#endif
