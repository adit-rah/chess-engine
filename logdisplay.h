#ifndef LOGDISPLAY_H
#define LOGDISPLAY_H

#include "display.h"
#include <fstream>
#include <string>

class LogDisplay : public AbstractDisplay {
    std::ofstream logFile;   // file stream for logging
    bool whiteToMove = true; // track move number formatting
    int moveCount = 0;

    std::string pieceTypeToChar(PieceType t) const;

public:
    LogDisplay(const std::string &filename = "chess_log.txt");
    ~LogDisplay() override;

    void notify(Subject &whoFrom) override;
};

#endif
