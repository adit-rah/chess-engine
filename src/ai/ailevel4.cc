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


// Alpha-beta minimax: evalOpponentMax = max (for opp), evalOpponentMin = min (for us)
const int INF = 100000;

int evalOpponentMin(Board& b, Colour myColour, int count, int alpha, int beta);

int evalOpponentMax(Board& b, Colour oppColour, int count, int alpha, int beta) {
    if (count == 0) return 0;
    Colour myColour = (oppColour == Colour::White) ? Colour::Black : Colour::White;
    int best = -INF;
    for (int row = 0; row < b.getBoardSize(); ++row) {
        for (int col = 0; col < b.getBoardSize(); ++col) {
            Position from(row, col);
            Piece* piece = b.getPieceAt(from);
            if (!piece || piece->getColour() != oppColour) continue;
            std::vector<Position> moves = piece->getValidMoves(b);
            for (Position to : moves) {
                int move_points = 0;
                Piece* target = b.getPieceAt(to);
                if (target && target->getColour() != oppColour && target->getColour() != Colour::None)
                    move_points += target->getValue();
                if (!b.makeMove(from, to)) continue;
                if (b.isCheckMate(myColour)) move_points += 1000;
                else if (b.isInCheck(myColour)) move_points += 1;
                int score = move_points - evalOpponentMin(b, myColour, count - 1, -beta, -alpha);
                b.unmakeMove();
                if (score > best) best = score;
                if (score > alpha) alpha = score;
                if (alpha >= beta) return best;
            }
        }
    }
    return (best == -INF) ? 0 : best;
}

int evalOpponentMin(Board& b, Colour myColour, int count, int alpha, int beta) {
    if (count == 0) return 0;
    Colour oppColour = (myColour == Colour::White) ? Colour::Black : Colour::White;
    int best = INF;
    for (int row = 0; row < b.getBoardSize(); ++row) {
        for (int col = 0; col < b.getBoardSize(); ++col) {
            Position from(row, col);
            Piece* piece = b.getPieceAt(from);
            if (!piece || piece->getColour() != myColour) continue;
            std::vector<Position> moves = piece->getValidMoves(b);
            for (Position to : moves) {
                int move_points = 0;
                Piece* target = b.getPieceAt(to);
                if (target && target->getColour() != myColour && target->getColour() != Colour::None)
                    move_points += target->getValue();
                if (!b.makeMove(from, to)) continue;
                if (b.isCheckMate(oppColour)) move_points += 1000;
                else if (b.isInCheck(oppColour)) move_points += 1;
                int score = move_points - evalOpponentMax(b, oppColour, count - 1, -beta, -alpha);
                b.unmakeMove();
                if (score < best) best = score;
                if (score < beta) beta = score;
                if (beta <= alpha) return best;
            }
        }
    }
    return (best == INF) ? 0 : best;
}

std::vector<Position> AILevel4::determineNextBestMove(Board &b) {
    Board searchBoard(b);
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
                        move_points = piece->getValue() - lowestAttackerValue;
                    }
                    Piece* target = b.getPieceAt(to);
                    if (target && target->getColour() == oppColour) {
                        int lowestOpponentDefender = getLowestAttackerValue(b, oppColour, to);
                        int lowestOurSupport = getLowestAttackerValue(b, colour, to, from);
                        bool targetIsDefended = (lowestOpponentDefender > 0);
                        bool weAreSupported = (lowestOurSupport > 0);
                        if (!targetIsDefended || (target->getValue() > piece->getValue())) {
                            move_points += target->getValue();
                        } else if (targetIsDefended && weAreSupported) {
                            move_points += target->getValue() - piece->getValue();
                        } else {
                            move_points -= piece->getValue();
                        }
                    }
                    if (!searchBoard.makeMove(from, to)) continue;
                    if (searchBoard.isCheckMate(oppColour)) move_points = 1000;
                    else if (searchBoard.isInCheck(oppColour)) move_points += 2;
                    int opponentBest = evalOpponentMax(searchBoard, oppColour, 2, -INF, INF);
                    searchBoard.unmakeMove();
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

