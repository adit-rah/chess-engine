#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"

class Player {
protected:
    Colour colour;
public:
    Player(Colour c) : colour{c} {}
    virtual ~Player() = default;

    Colour getColour() const { return colour; }

    virtual bool makeMove(Board &board) = 0;
};

#endif
