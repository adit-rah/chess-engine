#include "ailevel4.h"
#include "scoredposition.h"
#include <vector>
#include <algorithm>
#include <iostream>

// helper function to find the least vauable attacker of a square, with the option of excluding a specific position
int AILevel4::getLowestAttackerValue(Board &b, Colour oppColour, Position pos, Position exclude ) {
     int lowestValue = 100;
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


// Helper function to evaluate the best score the opponent can achieve after a move
int evaluateOpponentBestMove(Board &b, Colour oppColour, int count = 2) {
    if (count == 0) {
        return 0; // base case, static evaluation
    }
    int bestScore = -10000;
    Colour myColour = (oppColour == Colour::White) ? Colour::Black : Colour::White;
    for (int row = 0; row < b.getBoardSize(); ++row) {
        for (int col = 0; col < b.getBoardSize(); ++col) {
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
                        move_points += 1;
                    }
                    // Recursively evaluate the next move for the other player
                    int replyScore = -evaluateOpponentBestMove(tempBoard, myColour, count - 1);
                    int totalScore = move_points + replyScore;
                    if (totalScore > bestScore) bestScore = totalScore; // update best score
                }
            }
        }
    }
    return bestScore;
}

std::vector<Position> AILevel4::determineNextBestMove(Board &b) {
    Colour oppColour = (colour == Colour::White) ? Colour::Black : Colour::White;
    std::vector<ScoredPosition> scoredMoves;

    for (int row = 0; row < b.getBoardSize(); ++row) {
        for (int col = 0; col < b.getBoardSize(); ++col) {
            Position from(row, col);
            int lowestAttackerValue = getLowestAttackerValue(b, oppColour, from);
            Piece* piece = b.getPieceAt(from);
            if (piece && piece->getColour() == colour) {
                std::vector<Position> moves = piece->getValidMoves(b);
                for (Position to : moves) {
                    int move_points = 0;
                    if (lowestAttackerValue > 0) {
                        move_points = piece->getValue() - lowestAttackerValue; // tracks how important it is to move piece
                    }
                    Piece* target = b.getPieceAt(to);
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
                    // Simulate the move
                    Board tempBoard(b);
                    tempBoard.movePiece(from, to);
                    if (tempBoard.isCheckMate(oppColour)) {
                        move_points = 1000;
                    } else if (tempBoard.isInCheck(oppColour)) {
                        move_points += 2; // 2 points for putting opponent in check
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

