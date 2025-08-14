#ifndef AILEVEL5_H
#define AILEVEL5_H

#include "computerplayer.h"
#include "scoredposition.h"
#include <vector>

class AILevel5 : public ComputerPlayer {
public:
    AILevel5(Colour c, int maxDepth = 4) : ComputerPlayer(c), me(c), maxDepth(maxDepth) {}

    // Returns {from,to} or {-1,-1},{-1,-1} if no move
    std::vector<Position> determineNextBestMove(Board &b) override;

private:
    static constexpr int INF = 1000000000;
    Colour me;
    int maxDepth;

    static Colour opposite(Colour c);

    int eval(Board &b, Colour toMove) const;
    int pieceValue(PieceType t) const;

    std::vector<ScoredPosition> generateLegalMoves(Board &b, Colour side) const;
    void orderMoves(Board &b, Colour side, std::vector<ScoredPosition> &moves) const;

    int quiescence(Board &b, Colour toMove, int alpha, int beta);
    int negativemax(Board &b, Colour toMove, int depth, int alpha, int beta);
};

#endif
