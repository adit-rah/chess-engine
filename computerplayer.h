#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H
#include "PRNG.h"
#include "player.h"
#include "board.h"

class ComputerPlayer : public Player {
public:
    PRNG prng;  // it's specific pseudo random number generator 
    ComputerPlayer(Colour c) : Player(c) {}
    
    // the make move function is overriden to both ignore the additional arguements 
    // and to execute the best move calculated by the specific computer
    bool makeMove(Board &board, std::istringstream &iss) override;

    // used to determine the next best move (vector of size 2, from and to position)
    virtual std::vector<Position> determineNextBestMove(Board &b) = 0;
};

#endif
