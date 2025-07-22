#include "textdisplay.h"
#include "board.h"
#include "piece.h"
#include <iostream>

void TextDisplay::notify(Subject& whoFrom) {
    Board& board = dynamic_cast<Board&>(whoFrom);   // like it will only be board, but still

    std::cout << "  a b c d e f g h\n";

    for (int r = 7; r >= 0; --r) {
        std::cout << (r + 1) << " ";
        for (int c = 0; c < 8; ++c) {
            Position pos(r, c);
            Piece* p = board.getPieceAt(pos);
            if (!p || p->getType() == PieceType::None) {
                std::cout << ". ";
            } else {
                char symbol = p->getSymbol();
                // Optionally lowercase black pieces for clarity
                if (p->getColour() == Colour::Black)
                    symbol = tolower(symbol);
                std::cout << symbol << " ";
            }
        }
        std::cout << (r + 1) << "\n";
    }
    std::cout << "  a b c d e f g h\n\n";
}
