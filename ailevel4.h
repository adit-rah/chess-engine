#ifndef AILEVEL4_H
#define AILEVEL4_H

#include "computerplayer.h"

// refer to computerplayer.h for documentation
class AILevel4 : public ComputerPlayer {
public:
    AILevel4(Colour c) : ComputerPlayer(c) {}
    std::vector<Position> determineNextBestMove(Board &b) override;
    int getLowestAttackerValue(Board &b, Colour oppColour, Position pos, Position exclude = Position(-1,-1) );
};

#endif
