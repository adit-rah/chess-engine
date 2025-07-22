#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H

#include "player.h"
#include "board.h"

class ComputerPlayer : public Player {
public:
    ComputerPlayer(Colour c) : Player(c) {}
    bool makeMove(Board &board) override;

    virtual Position determineNextBestMove(Board &b) = 0;
};

#endif
