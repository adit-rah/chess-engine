#ifndef SCOREDPOSITION_H
#define SCOREDPOSITION_H
#include "piece.h"

class ScoredMove : {
    public:
    Position to;
    Position from;
        int score;
        ScoredPosition(Position from, int r = 0, int c = 0, int s = 0) : 
            to(Position(r, c)), score(s), from(from) {}
        ScoredPosition(Position from, Position to, int s) :
            to(to), score(s), from(from) {}
        bool operator<(const ScoredPosition& other) const {
            return score < other.score;
        }
};