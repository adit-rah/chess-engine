#ifndef COMPUTERPLAYER_H
#define COMPUTERPLAYER_H
#include "PRNG.h"
#include "player.h"
#include "board.h"
#include 
class ComputerPlayer : public Player {
public:
    ComputerPlayer(Colour c) : Player(c) {}
    
    bool makeMove(Board &boardbool, std::istringstream &iss) override {
    std::vector<Position> move = determineNextBestMove(board);
    if (move.size() != 2) return false;
    Piece* piece = board.getPieceAt(move[0]);
    
    if (piece && piece->getColour() == colour) {
        // Make the move on the board
        return board.movePiece(move[0], move[1]);
    }
    return false; // If no valid move was made
}

    virtual std::vector<Position,2> determineNextBestMove(Board &b) = 0;
};

#endif
