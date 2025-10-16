#ifndef SCOREDPOSITION_H
#define SCOREDPOSITION_H
#include "piece.h"

// just a scored move
class ScoredPosition {
public:
    Position to;
    Position from;
    int score;          // holds a score
    
    ScoredPosition(Position from, int r = 0, int c = 0, int s = 0);
    ScoredPosition(Position from, Position to, int s);
    bool operator<(const ScoredPosition& other) const;
};

#endif
