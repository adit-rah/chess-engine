#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"

#include <sstream>

class Player {
protected:
    Colour colour;
public:
    Player(Colour c) : colour{c} {}
    virtual ~Player() = default;

    Colour getColour() const { return colour; }

    virtual bool makeMove(Board &board, std::istringstream &iss) = 0;
};

#endif
