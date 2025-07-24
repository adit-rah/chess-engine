#include "ailevel3.h"
#include "scoredposition.h"
#include <vector>
#include <algorithm>

// helper function to find the least vauable attacker of a square
// ADD TO .H AFTER
static int getLowestAttackerValue(Board &b, Colour oppColour, Position pos ){
    int lowestValue = 100; // set to a high value, since no piece can have a value higher than 10
    for (int row = 0; row < 8; row ++){
        for (int col = 0; col < 8; col++){
            Position attackerPos(row, col);
            Piece* attacker = b.getPieceAt(attackerPos);
            if (attacker && attacker->getColour() == oppColour) {
                std::vector<Position> validMoves = attacker->getValidMoves(b);
                if (std::find(validMoves.begin(), validMoves.end(), pos) != validMoves.end()) {
                    // update lowest value if the current attacker is less valuable
                    lowestValue = (lowestValue > attacker->getValue()) ?
                                   attacker->getValue() : lowestValue;
                }
            }
        }
    }
    return lowestValue == 100 ? 0 : lowestValue; // if no attacker found, return 0
}

std::vector<Position> AILevel3::determineNextBestMove(Board &b) {
    static PRNG prng;                              // remember you did this earlier (adit/ just in case)

    std::vector<ScoredPosition> scoredMoves;
    Colour oppColour = (colour == Colour::White) ? Colour::Black : Colour::White;
    // store all squares currently being attacked by the opponent
    std::vector<Position> dangerSquares = b.squaresBeingAttackedBy(oppColour);
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Position from(row, col);
            int lowestAttackerValue = getLowestAttackerValue(b, oppColour, from);

            Piece* piece = b.getPieceAt(from);
            if (piece && piece->getColour() == colour) {
                std::vector<Position> moves = piece->getValidMoves(b);
                for (Position to : moves) {
                    int move_points = piece->getValue() - lowestAttackerValue; // tracks how important it is to move piece
                    Piece* target = b.getPieceAt(to);
                    // Add points for capturing
                    if (target && target->getColour() == oppColour) {
                        move_points += target->getValue(); // increase value for capturing an opponent's piece
                        // if piece is defended, avoid capturing it unless it unless it is a favourable trade
                            lowestAttackerValue = getLowestAttackerValue(b, oppColour, to);
                            if (lowestAttackerValue > 0) {
                                move_points -= target->getValue(); // account for a trade from opponent
                            
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
                        move_points += 5;   // 5 points for putting opponent in check
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

