#include "ailevel2.h"
#include "scoredposition.h"
#include "PRNG.h"
#include <vector>
#include <algorithm>

// Assumes ScoredPosition has: Position from, Position to, int score

std::vector<Position> AILevel2::determineNextBestMove(Board &b) {
    std::vector<ScoredPosition> scoredMoves;
    Colour oppColour = (colour == Colour::White) ? Colour::Black : Colour::White;

    for (int row = 0; row < b.getBoardSize(); ++row) {
        for (int col = 0; col < b.getBoardSize(); ++col) {
            Piece* piece = b.getPieceAt(Position(row, col));
            if (piece && piece->getColour() == colour) {
                std::vector<Position> moves = piece->getValidMoves(b);
                for (Position to : moves) {
                    int move_points = 0;
                    Piece* target = b.getPieceAt(to);
                    // Add points for capturing
                    if (target && target->getColour() == oppColour) {
                        move_points += target->getValue(); // 1 point for capturing an opponent's piece
                    }
                    
                    // Make a copy of the board
                    Board tempBoard(b);
                    // Apply the move on the copy
                    tempBoard.movePiece(Position(row, col), to);

                    if (tempBoard.isCheckMate(oppColour)){
                        move_points = 1000; // 1000 points for checkmate
                    } 
                    if (tempBoard.isInCheck(oppColour)) {
                        move_points += 5;   // 5 points for putting opponent in check
                    } 

                    scoredMoves.emplace_back(Position(row, col), to, move_points);
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

