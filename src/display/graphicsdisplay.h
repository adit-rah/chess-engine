#ifndef GRAPHICSDISPLAY_H
#define GRAPHICSDISPLAY_H

#include "display.h"
#include "window.h"

// Graphical display for the chessboard
class GraphicsDisplay : public AbstractDisplay {
    Xwindow window;
    int windowSize;
    int squareSize;
    const int labelMargin;
    char lastBoard[8][8];   // cached symbols (or '_' for empty)
public:
    GraphicsDisplay(int size = 500);

    void drawStaticBoard();
    void drawPiece(int r, int c, char sym);

    void notify(Subject &whoFrom) override;
};


#endif
