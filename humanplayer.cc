#include "humanplayer.h"
#include "board.h"
#include <iostream>
#include <sstream>

// helper: convert algebraic notation ("e2") -> Position(row, col)
static Position parseAlgebraic(const std::string &sq) {
    if (sq.size() != 2) return Position(-1, -1);
    int col = sq[0] - 'a';
    int row = sq[1] - '1';
    return Position(row, col);
}

bool HumanPlayer::makeMove(Board &board, std::istringstream &iss) {
    std::string fromSq, toSq, promotionChoice;
    iss >> fromSq >> toSq;

    if (fromSq.empty() || toSq.empty()) {
        std::cout << "Invalid input.\n";
        return false;
    }

    if (iss.peek() != '\n') {
        iss >> promotionChoice;
    }

    Position from = parseAlgebraic(fromSq);
    Position to = parseAlgebraic(toSq);

    Piece* piece = board.getPieceAt(from);
    if (!piece) {
        std::cout << "No piece at " << fromSq << "\n";
        return false;
    }
    if (piece->getColour() != colour) {
        std::cout << "That’s not your piece!\n";
        return false;
    }

    board.setPendingPromotion(promotionChoice.empty() ? 'Q' : promotionChoice[0]);

    return board.movePiece(from, to);
}

