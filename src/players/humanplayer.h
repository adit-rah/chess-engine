#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "player.h"

class HumanPlayer : public Player {
public:
    HumanPlayer(Colour c) : Player(c) {}

    // makes moves in accordance to the input
    bool makeMove(Board &board, std::istringstream &iss) override;
};

#endif
