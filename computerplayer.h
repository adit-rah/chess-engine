#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H
#include "PRNG.h"
#include "player.h"
#include "board.h"

class ComputerPlayer : public Player {
public:
    PRNG prng; 
    ComputerPlayer(Colour c) : Player(c) {}
    
    bool makeMove(Board &board, std::istringstream &iss) override;
    virtual std::vector<Position> determineNextBestMove(Board &b) = 0;
};

#endif
