#include "board.h"
#include "piece.h"

Board() {
     pieces = new Piece**[8];
        for (int i = 0; i < 8; ++i) {
            pieces[i] = new Piece*[8];
            for (int j = 0; j < 8; ++j) {
                pieces[i][j] = createPiece(i, j);
            }
        }
}

Piece* Board::getPieceAt(Position p) {
    if (p.row < 0 || p.row >= 8 || p.col < 0 || p.col >= 8) {
        return nullptr; 
    }
    return pieces[p.row][p.col];
}

bool Board::movePiece(Position from, Position to) {
    Piece* piece = getPieceAt(from);
    if (!piece) return false;
    std::vector<Position> validMoves = piece->getValidMoves();
    for (Position move : validMoves){
        if (move == to) {
            Piece * targetPiece = getPieceAt(to);
            // add inrecemnting player points here later
            pieces[to.row][to.col] = piece;
            // replace old position with empty piece
            pieces[from.row][from.col] = new Pawn(Color::None, from);
            // delete captured piece
            delete targetPiece; 
            return true;

            }
        }
        return false; // Invalid move
    }
bool Board::isCheckMate(Color c) {
    // Check if the player has any valid moves left
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Piece* piece = pieces[i][j];
            if (piece && piece->getColour() == c) {
                if (canMove(*piece)) {
                    return false; // Found a valid move, not checkmate
                }
            }
        }
    }
    return true; // No valid moves left, it's checkmate
}

bool Board::canMove(Piece& p) {
    std::vector<Position> validMoves = p.getValidMoves();
    return !validMoves.empty();
}

std::vector<Position> Board::squaresBeingAttacked(Color c) {
    std::vector<Position> attackedSquares;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Piece* piece = pieces[i][j];
            if (piece && piece->getColour() != c) {
                std::vector<Position> validMoves = piece->getValidMoves();
                attackedSquares.insert(attackedSquares.end(), validMoves.begin(), validMoves.end());
            }
        }
    }
    return attackedSquares;
}