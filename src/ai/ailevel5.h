#ifndef AILEVEL5_H
#define AILEVEL5_H

#include "computerplayer.h"

// Strongest AI: 4-ply alpha-beta minimax, static eval, piece-square tables,
// move ordering, and quiescence search.
class AILevel5 : public ComputerPlayer {
public:
    AILevel5(Colour c) : ComputerPlayer(c) {}

    std::vector<Position> determineNextBestMove(Board &b) override;
};

#endif
