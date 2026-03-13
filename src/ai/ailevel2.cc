#include "ailevel2.h"
#include "scoredposition.h"
#include "PRNG.h"
#include <vector>
#include <algorithm>

// Assumes ScoredPosition has: Position from, Position to, int score

std::vector<Position> AILevel2::determineNextBestMove(Board &b) {
    Board searchBoard(b);
    std::vector<ScoredPosition> scoredMoves;
    Colour oppColour = (colour == Colour::White) ? Colour::Black : Colour::White;

    for (int row = 0; row < b.getBoardSize(); ++row) {
        for (int col = 0; col < b.getBoardSize(); ++col) {
            Position from(row, col);
            Piece* piece = b.getPieceAt(from);
            if (piece && piece->getColour() == colour) {
                std::vector<Position> moves = piece->getValidMoves(b);
                for (Position to : moves) {
                    int move_points = 0;
                    Piece* target = b.getPieceAt(to);
                    if (target && target->getColour() == oppColour) {
                        move_points += target->getValue();
                    }
                    if (!searchBoard.makeMove(from, to)) continue;
                    if (searchBoard.isCheckMate(oppColour)) move_points = 1000;
                    if (searchBoard.isInCheck(oppColour)) move_points += 5;
                    searchBoard.unmakeMove();
                    scoredMoves.emplace_back(from, to, move_points);
                }
            }
        }
    }

    if (scoredMoves.empty()) {
        return {Position(-1, -1), Position(-1, -1)};
    }

    // Find the max score
    int maxScore = std::max_element(
        scoredMoves.begin(), scoredMoves.end(),
        [](const ScoredPosition& a, const ScoredPosition& b) { return a.score < b.score; }
    )->score;

    // Collect all moves with the max score
    std::vector<ScoredPosition> bestMoves;
    for ( ScoredPosition& m : scoredMoves) {
        if (m.score == maxScore) bestMoves.push_back(m);
    }

    if (bestMoves.empty()) {
        return {Position(-1, -1), Position(-1, -1)}; // No valid moves found
    }    
    // Pick one at random if there are multiple
    int idx = prng(0, bestMoves.size() - 1);
    return {bestMoves[idx].from, bestMoves[idx].to};

}

