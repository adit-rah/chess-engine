#ifndef PLAYER_H
#define PLAYER_H

#include "board.h"


class Player {
protected:
    Colour colour;   // White or Black

public:
    Player(Colour c) : colour{c} {}
    virtual ~Player() = default;

    Colour getColour() const { return colour; }

    virtual bool makeMove(Board &board) = 0;
};


class HumanPlayer : public Player {
public:
    HumanPlayer(Colour c) : Player(c) {}
    bool makeMove(Board &board) override;
};


class ComputerPlayer : public Player {
public:
    ComputerPlayer(Colour c) : Player(c) {}
    bool makeMove(Board &board) override;

    virtual Position determineNextBestMove(Board &b) = 0;
};


// Just the base AI in this, we
class AILevel1 : public ComputerPlayer {
public:
    AILevel1(Colour c) : ComputerPlayer(c) {}
    bool makeMove(Board &board) override; // picks a random valid move
};

#endif
