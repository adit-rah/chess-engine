#include "ailevel4.h"
#include "scoredposition.h"
#include <vector>
#include <algorithm>

// helper function to find the least vauable attacker of a square
// ADD TO .H AFTER
int getLowestAttackerValue(Board &b, Colour oppColour, Position pos ){
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


// Helper function to evaluate the best score the opponent can achieve after a move
int evaluateOpponentBestMove(Board &b, Colour oppColour, int count = 3) {
    if (count == 0) {
        return 0; // base case, static evaluation
    }
    int bestScore = -10000;
    Colour myColour = (oppColour == Colour::White) ? Colour::Black : Colour::White;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Position from(row, col);
            Piece* piece = b.getPieceAt(from);
            if (piece && piece->getColour() == oppColour) {
                std::vector<Position> moves = piece->getValidMoves(b);
                for (Position to : moves) {
                    int move_points = 0;
                    Piece* target = b.getPieceAt(to);
                    if (target && target->getColour() != oppColour && target->getColour() != Colour::None) {
                        move_points += target->getValue();
                    }
                    Board tempBoard(b);
                    tempBoard.movePiece(from, to);
                    if (tempBoard.isCheckMate(myColour)) {
                        move_points += 1000;
                    } else if (tempBoard.isInCheck(myColour)) {
                        move_points += 5;
                    }
                    // Recursively evaluate the next move for the other player
                    int replyScore = evaluateOpponentBestMove(tempBoard, myColour, count - 1);
                    int totalScore = move_points - replyScore;
                    if (totalScore > bestScore) bestScore = totalScore; // update best score
                }
            }
        }
    }
    return bestScore;
}

std::vector<Position> AILevel4::determineNextBestMove(Board &b) {
    static PRNG prng;
    Colour oppColour = (colour == Colour::White) ? Colour::Black : Colour::White;
    std::vector<ScoredPosition> scoredMoves;

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Position from(row, col);
            int lowestAttackerValue = getLowestAttackerValue(b, oppColour, from);
            Piece* piece = b.getPieceAt(from);
            if (piece && piece->getColour() == colour) {
                std::vector<Position> moves = piece->getValidMoves(b);
                for (Position to : moves) {
                    int move_points = piece->getValue() - lowestAttackerValue;
                    Piece* target = b.getPieceAt(to);
                    if (target && target->getColour() == oppColour) {
                        move_points += target->getValue();
                        int lowestOpponentDefender = getLowestAttackerValue(b, oppColour, to);
                        if (lowestOpponentDefender > 0) {
                            move_points -= target->getValue();
                        }
                    }
                    // Simulate the move
                    Board tempBoard(b);
                    tempBoard.movePiece(from, to);
                    if (tempBoard.isCheckMate(oppColour)) {
                        move_points = 1000;
                    } else if (tempBoard.isInCheck(oppColour)) {
                        move_points += 5;
                    }
                    // Now, check the opponent's best reply
                    int opponentBest = evaluateOpponentBestMove(tempBoard, oppColour);
                    // We want to minimize the opponent's best score
                    move_points -= opponentBest;
                    
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
    for (ScoredPosition& m : scoredMoves) {
        if (m.score == maxScore) bestMoves.push_back(m);
    }

    if (bestMoves.empty()) {
        return {Position(-1, -1), Position(-1, -1)};
    }
    int idx = prng(0, bestMoves.size() - 1);
    return {bestMoves[idx].from, bestMoves[idx].to};
}

