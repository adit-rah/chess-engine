#include "ailevel1.h"
#include "scoredposition.h"
#include "PRNG.h"

std::vector<Position> AILevel1::determineNextBestMove(Board &b){
    static PRNG prng;                              // remember you did this earlier (adit/ just in case)

    std::vector<ScoredPosition> allMoves;
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* piece = b.getPieceAt(Position(row, col));
            if (piece && piece->getColour() == colour) {
                std::vector<Position> moves = piece->getValidMoves(b);
                for (auto& move : moves){
                    allMoves.emplace_back(Position(row,col),move, 0); // Don't trakc score for Lvl1 AI
                }
            }
        }
    }
    std::vector<Position> move;
    if (allMoves.empty()) {
        move = {Position(-1,-1), Position(-1,-1)}; // No valid Moves available
    }
    else{
        int randomPos = prng(0, allMoves.size() - 1);
        ScoredPosition chosen = allMoves[randomPos];
        move.emplace_back(chosen.from); 
        move.emplace_back(chosen.to); // Randomly select a piece to move
       
    }
    return move;
}

