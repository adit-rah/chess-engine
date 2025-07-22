#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H
#include "PRNG.h"
#include "player.h"
#include "board.h"
#include 
class ComputerPlayer : public Player {
public:
    ComputerPlayer(Colour c) : Player(c) {}
    bool makeMove(Board &board) override;

    virtual std::vector<Position,2> determineNextBestMove(Board &b) = 0;
};

#endif
