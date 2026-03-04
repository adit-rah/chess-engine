#ifndef AILEVEL1_H
#define AILEVEL1_H

#include "computerplayer.h"

// refer to computerplayer.h for documentation
class AILevel1 : public ComputerPlayer {
public:
    AILevel1(Colour c) : ComputerPlayer(c) {}

    std::vector<Position> determineNextBestMove(Board &b) override;
};

#endif
