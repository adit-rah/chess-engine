#ifndef AILEVEL2_H
#define AILEVEL2_H

#include "computerplayer.h"

// refer to computerplayer.h for documentation
class AILevel2 : public ComputerPlayer {
public:
    AILevel2(Colour c) : ComputerPlayer(c) {}
    
    std::vector<Position> determineNextBestMove(Board &b) override;
};

#endif
