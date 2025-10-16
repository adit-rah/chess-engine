#include "computerplayer.h"

bool ComputerPlayer::makeMove(Board &board, std::istringstream &iss) {
    std::vector<Position> move = determineNextBestMove(board);
    if (move.size() != 2) return false;
    Piece* piece = board.getPieceAt(move[0]);
    
    if (piece && piece->getColour() == colour) {
        // Make the move on the board
        return board.movePiece(move[0], move[1]);
    }
    return false; // If no valid move was made
}
