#ifndef HUMANPLAYER_H
#define HUMANPLAYER_H

#include "player.h"

class HumanPlayer : public Player {
public:
    HumanPlayer(Colour c) : Player(c) {}
    bool makeMove(Board &board) override;
};

#endif
