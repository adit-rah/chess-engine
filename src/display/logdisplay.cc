#include "logdisplay.h"
#include "board.h"
#include "piece.h"
#include <iostream>

LogDisplay::LogDisplay(const std::string &filename) {
    logFile.open(filename, std::ios::out | std::ios::trunc);
    if (!logFile) {
        std::cerr << "Failed to open log file: " << filename << std::endl;
    }
}

LogDisplay::~LogDisplay() {
    if (logFile.is_open()) {
        logFile.close();
    }
}


std::string LogDisplay::pieceTypeToChar(PieceType t) const { // working with the information we got
    switch (t) {
        case PieceType::Pawn:   return "";   // pawns don’t have letter in algebraic
        case PieceType::Knight: return "N";
        case PieceType::Bishop: return "B";
        case PieceType::Rook:   return "R";
        case PieceType::Queen:  return "Q";
        case PieceType::King:   return "K";
        default: return "";
    }
}

void LogDisplay::notify(Subject &whoFrom) {
    if (!logFile.is_open()) return;

    Board& board = dynamic_cast<Board&>(whoFrom);

    Position from = board.getLastMoveFrom();
    Position to = board.getLastMoveTo();
    PieceType movedType = board.getLastMovePieceType();

    // Convert positions
    char file = 'a' + from.col;
    char rank = '1' + from.row;
    std::string fromStr = std::string{file,rank};
    file = 'a' + to.col;
    rank = '1' + to.row;
    std::string toStr = std::string{file,rank};

    // minimal algebraic notation: e2e4 or Nf3
    std::string moveText = pieceTypeToChar(movedType) + toStr;

    if (whiteToMove) {
        ++moveCount;
        logFile << moveCount << ". " << moveText << " ";
    } else {
        logFile << moveText << "\n";
    }

    logFile.flush();  // ensure it writes immediately
    whiteToMove = !whiteToMove;
}
