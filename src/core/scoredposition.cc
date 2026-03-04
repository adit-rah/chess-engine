#include "scoredposition.h"


ScoredPosition::ScoredPosition(Position from, int r, int c, int s) : 
    to(Position(r, c)), from(from), score(s) {}


ScoredPosition::ScoredPosition(Position from, Position to, int s) :
    to(to), from(from), score(s) {}


bool ScoredPosition::operator<(const ScoredPosition& other) const {
    return score < other.score;
}
