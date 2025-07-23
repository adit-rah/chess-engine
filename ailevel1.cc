#include "ailevel1.h"



std::vector<Position> AILevel1::determineNextBestMove(Board &b){
    std::vector<Position> startPos;
    std::vector<std::vector<Position>> allMoves;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = b.getPieceAt(Position(row, col));
            if (piece && piece->getColour() == colour) {
                std::vector<Position> moves = piece->getValidMoves(b);
                if (!moves.empty()) {
                    allMoves.push_back(moves);
                    startPos.push_back(Position(row, col));
                }
            }
        }
    }
    std::vector<Position> move;
    if (startPos.empty()) {
        move = {Position(-1,-1), Position(-1,-1)}; // No valid startPos available
    }
    else{
        int randomPos = prng(0, startPos.size() - 1);
        Position to = allMoves[randomPos][prng(0, allMoves[randomPos].size() - 1)];
        move.emplace_back(startPos[randomPos]); // Randomly select a piece to move
        move.emplace_back(to); // Randomly select a move for that piece
    }
    return move;
}

bool AILevel1::makeMove(Board &board) {
    std::vector<Position> move = determineNextBestMove(board);
    if (move.size() != 2) return false;
    Piece* piece = board.getPieceAt(move[0]);
    
    if (piece && piece->getColour() == colour) {
        // Make the move on the board
        return board.movePiece(move[0], move[1]);
    }
    return false; // If no valid move was made
}