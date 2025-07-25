#include "graphicsdisplay.h"
#ifdef None
#undef None

#include "board.h"
#include "piece.h"
#include <iostream> 

GraphicsDisplay::GraphicsDisplay(int size)
    : window(size + 40, size + 40),          // extra margin for labels
      windowSize(size),
      squareSize(size / 8),
      labelMargin(20)                        // space for labels
{
    // initialize lastBoard to empty
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            lastBoard[r][c] = '\0';
        }
    }

    // draw the static background ONCE
    drawStaticBoard();
}

void GraphicsDisplay::drawStaticBoard() {
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            int screenRow = 7 - r; // flip vertically
            int x = labelMargin + c * squareSize;
            int y = labelMargin + screenRow * squareSize;

            bool isLightSquare = (r + c) % 2 == 0;
            int colour = isLightSquare ? Xwindow::LightBrown : Xwindow::DarkBrown;

            window.fillRectangle(x, y, squareSize, squareSize, colour);
        }
    }

    // rank labels (1–8, bottom to top)
    for (int r = 0; r < 8; ++r) {
        int screenRow = 7 - r;
        int yCenter = labelMargin + screenRow * squareSize + squareSize / 2;
        char rankChar = '1' + r;
        window.drawString(5, yCenter, std::string(1, rankChar), Xwindow::White);
    }

    // file labels (a–h) still same
    for (int c = 0; c < 8; ++c) {
        int xCenter = labelMargin + c * squareSize + squareSize / 3;
        int yBottom = labelMargin + 8 * squareSize + 15;
        char fileChar = 'a' + c;
        window.drawString(xCenter, yBottom, std::string(1, fileChar), Xwindow::White);
    }

    window.flush();
}


void GraphicsDisplay::drawPiece(int r, int c, char sym) {
    int x = labelMargin + c * squareSize + squareSize / 3;
    int y = labelMargin + r * squareSize + squareSize / 2;
    window.drawString(x, y, std::string(1, sym), Xwindow::Black);
}


void GraphicsDisplay::notify(Subject &whoFrom) {
    Board &board = dynamic_cast<Board &>(whoFrom);

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece *piece = board.getPieceAt(Position(r, c));
            char symbol;

            if (!piece || piece->getType() == PieceType::None) {
                symbol = ((r + c) % 2 == 0) ? ' ' : '_';
            } else {
                symbol = piece->getSymbol();
                if (piece->getColour() == Colour::Black)
                    symbol = std::tolower(symbol);
                else
                    symbol = std::toupper(symbol);
            }

            if (symbol != lastBoard[r][c]) {
                int screenRow = 7 - r; // flip vertically
                int x = labelMargin + c * squareSize;
                int y = labelMargin + screenRow * squareSize;

                bool isLightSquare = (r + c) % 2 == 0;
                int colour = isLightSquare ? Xwindow::LightBrown : Xwindow::DarkBrown;
                window.fillRectangle(x, y, squareSize, squareSize, colour);

                if (symbol != ' ' && symbol != '_') {
                    // Also flip when drawing the piece
                    int pieceY = y + squareSize / 2;
                    int pieceX = x + squareSize / 2;
                    window.drawString(pieceX, pieceY, std::string(1, symbol), Xwindow::Black);
                }

                lastBoard[r][c] = symbol;
            }
        }
    }

    window.flush();
}

#endif
