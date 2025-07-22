#include "textdisplay.h"
#include "board.h"
#include "piece.h"
#include <iostream>


void TextDisplay::notify(Subject& whoFrom) {
    Board& board = dynamic_cast<Board&>(whoFrom);   // like it will only be board, but still
    // Board is 8x8; print rows 8 to 1
    for (int r = 7; r >= 0; --r) {
        std::cout << (r + 1) << " ";
        for (int c = 0; c < 8; ++c) {
            Piece* piece = board.getPieceAt(Position(r, c));
            char symbol = '_'; // default for dark squares without piece
            if (piece && piece->getType() != PieceType::None) {
                symbol = piece->getSymbol();
                // Capital for white, lowercase for black
                if (piece->getColour() == Colour::Black) {
                    symbol = std::tolower(symbol);
                } else {
                    symbol = std::toupper(symbol);
                }
            } else {
                // No piece, show blank space for white square and underscore for black square
                // Chessboard color: (row + col) even = light square
                if ((r + c) % 2 == 0)
                    symbol = ' ';
                else
                    symbol = '_';
            }
            std::cout << symbol;
        }
        std::cout << "\n";
    }
    std::cout << "abcdefgh\n";

    // Display check/checkmate/stalemate status:
    if (board.isInCheck(Colour::White)) {
        std::cout << "White is in check.\n";
    }
    if (board.isInCheck(Colour::Black)) {
        std::cout << "Black is in check.\n";
    }
    if (board.isCheckMate(Colour::White)) {
        std::cout << "Checkmate! Black wins!\n";
    } else if (board.isCheckMate(Colour::Black)) {
        std::cout << "Checkmate! White wins!\n";
    }
    // For stalemate detection, you can add an isStalemate method to Board and check here
    // if (board.isStalemate()) {
    //     std::cout << "Stalemate!\n";
    // }
}
