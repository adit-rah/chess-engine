#ifndef AILEVEL1_H
#define AILEVEL1_H

#include "computerplayer.h"

class AILevel1 : public ComputerPlayer {
public:
    AILevel1(Colour c) : ComputerPlayer(c) {}
    bool makeMove(Board &board, std::istringstream &iss) override;
};

#endif
