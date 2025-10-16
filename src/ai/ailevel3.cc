#include "ailevel3.h"
#include "scoredposition.h"
#include <vector>
#include <algorithm>
#include <iostream>

using namespace std;
// helper function to find the least vauable attacker of a square
// ADD TO .H AFTER
int AILevel3::getLowestAttackerValue(Board &b, Colour oppColour, Position pos, Position exclude ) {
     int lowestValue = 100;
     Colour colour = (oppColour == Colour::White) ? Colour::Black : Colour::White;
    for (int row = 0; row < b.getBoardSize(); row++) {
        for (int col = 0; col < b.getBoardSize(); col++) {
            Position attackerPos(row, col);

            // Exclude a specific position (like the one we're moving from)
            if (attackerPos == exclude) continue;

            Piece* attacker = b.getPieceAt(attackerPos);
            if (attacker && attacker->getColour() == colour) {
                std::vector<Position> validMoves = attacker->getValidMoves(b);
                if (std::find(validMoves.begin(), validMoves.end(), pos) != validMoves.end()) {
                    lowestValue = std::min(lowestValue, attacker->getValue());
                }
            }
        }
    }
    return (lowestValue == 100) ? 0 : lowestValue; // if no attacker found return 0
}

std::vector<Position> AILevel3::determineNextBestMove(Board &b) {
    std::vector<ScoredPosition> scoredMoves;
    Colour oppColour = (colour == Colour::White) ? Colour::Black : Colour::White;
    // store all squares currently being attacked by the opponent
    std::vector<Position> dangerSquares = b.squaresBeingAttackedBy(oppColour);
    for (int row = 0; row < b.getBoardSize(); ++row) {
        for (int col = 0; col < b.getBoardSize(); ++col) {
            Position from(row, col);
            int lowestAttackerValue = getLowestAttackerValue(b, oppColour, from);

            Piece* piece = b.getPieceAt(from);
            if (piece && piece->getColour() == colour) {
                std::vector<Position> moves = piece->getValidMoves(b);
                for (Position to : moves) {
                    int move_points = 0;
                    if (lowestAttackerValue > 0){
                        move_points = piece->getValue() - lowestAttackerValue; // tracks how important it is to move piece
                    }
                    Piece* target = b.getPieceAt(to);
                    // Add points for capturing
                    if (target && target->getColour() == oppColour) {
                        int lowestOpponentDefender = getLowestAttackerValue(b, oppColour, to);
                        int lowestOurSupport = getLowestAttackerValue(b, colour, to, from);
                        bool targetIsDefended = (lowestOpponentDefender > 0);
                        bool weAreSupported = (lowestOurSupport > 0);

                        if (!targetIsDefended || (target->getValue() > piece->getValue())) {
                            move_points += target->getValue(); // capture
                        } else if (targetIsDefended && weAreSupported) {
                            move_points += target->getValue() - piece->getValue();
                        } else {
                            move_points -= piece->getValue(); // risky trade
                        }
                    }
                    
                    // Make a copy of the board
                    Board tempBoard(b);
                    // Apply the move on the copy
                    tempBoard.movePiece(Position(row, col), to);

                    if (tempBoard.isCheckMate(oppColour)){
                        move_points = 1000; // gain 1000 points to never miss checkmate
                    } 
                    else if (tempBoard.isInCheck(oppColour)) {
                        move_points += 2;   // 2 points for putting opponent in check
                    }

                    

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

