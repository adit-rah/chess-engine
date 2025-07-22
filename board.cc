#include "board.h"
#include "piece.h"
#include "pawn.h"
#include "rook.h"
#include "knight.h"
#include "bishop.h"
#include "queen.h"
#include "king.h"


Board::Board() {
    pieces = new Piece**[8];
    for (int i = 0; i < 8; ++i) {
        pieces[i] = new Piece*[8];
        for (int j = 0; j < 8; ++j) {
            pieces[i][j] = nullptr;
        }
    }
}


void Board::setPieces() {
    // === WHITE PIECES ===
    // Pawns
    for (int c = 0; c < 8; ++c) {
        pieces[1][c] = new Pawn(Colour::White, Position(1, c));
    }
    // Back row
    pieces[0][0] = new Rook(Colour::White, Position(0,0));
    pieces[0][7] = new Rook(Colour::White, Position(0,7));
    pieces[0][1] = new Knight(Colour::White, Position(0,1));
    pieces[0][6] = new Knight(Colour::White, Position(0,6));
    pieces[0][2] = new Bishop(Colour::White, Position(0,2));
    pieces[0][5] = new Bishop(Colour::White, Position(0,5));
    pieces[0][3] = new Queen(Colour::White, Position(0,3));
    pieces[0][4] = new King(Colour::White, Position(0,4));

    // === BLACK PIECES ===
    // Pawns
    for (int c = 0; c < 8; ++c) {
        pieces[6][c] = new Pawn(Colour::Black, Position(6, c));
    }
    // Back row
    pieces[7][0] = new Rook(Colour::Black, Position(7,0));
    pieces[7][7] = new Rook(Colour::Black, Position(7,7));
    pieces[7][1] = new Knight(Colour::Black, Position(7,1));
    pieces[7][6] = new Knight(Colour::Black, Position(7,6));
    pieces[7][2] = new Bishop(Colour::Black, Position(7,2));
    pieces[7][5] = new Bishop(Colour::Black, Position(7,5));
    pieces[7][3] = new Queen(Colour::Black, Position(7,3));
    pieces[7][4] = new King(Colour::Black, Position(7,4));

    // === EMPTY SQUARES ===
    for (int r = 2; r <= 5; ++r) {
        for (int c = 0; c < 8; ++c) {
            pieces[r][c] = nullptr; // or new EmptyPiece if you prefer
        }
    }
}


Piece* Board::getPieceAt(Position p) const {
    if (p.row < 0 || p.row >= 8 || p.col < 0 || p.col >= 8) {
        return nullptr; 
    }
    return pieces[p.row][p.col];
}


bool Board::movePiece(Position from, Position to) {
    Piece* piece = getPieceAt(from);
    if (!piece) return false;
    std::vector<Position> validMoves = piece->getValidMoves(*this);
    for (Position move : validMoves){
        if (move == to) {
            Piece * targetPiece = getPieceAt(to);
            // add inrecemnting player points here later
            pieces[to.row][to.col] = piece;
            // replace old position with empty piece
            pieces[from.row][from.col] = new Pawn(Colour::None, from);
            // delete captured piece
            delete targetPiece;

            return true;
        }
    }
    return false; // Invalid move
}


bool Board::isCheckMate(Colour c) const {
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

bool Board::canMove(Piece& p) const {
    std::vector<Position> validMoves = p.getValidMoves();
    return !validMoves.empty();
}

std::vector<Position> Board::squaresBeingAttacked(Colour c) const {
    std::vector<Position> attackedSquares;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Piece* piece = pieces[i][j];
            if (piece && piece->getColour() != c) {
                std::vector<Position> validMoves = piece->getValidMoves(*this);
                attackedSquares.insert(attackedSquares.end(), validMoves.begin(), validMoves.end());
            }
        }
    }
    return attackedSquares;
}
