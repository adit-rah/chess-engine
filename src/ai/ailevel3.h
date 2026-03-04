#ifndef AILEVEL3_H
#define AILEVEL3_H

#include "computerplayer.h"

// refer to computerplayer.h for documentation
class AILevel3 : public ComputerPlayer {
public:
    AILevel3(Colour c) : ComputerPlayer(c) {}
    
    int getLowestAttackerValue(Board &b, Colour oppColour, Position pos, Position exclude = Position(-1,-1) );
    std::vector<Position> determineNextBestMove(Board &b) override;
};

#endif
