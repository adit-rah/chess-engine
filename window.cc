#include "window.h"
#include <X11/Xutil.h>
#include <iostream>
#include <cstdlib>
#include <unistd.h>

Xwindow::Xwindow(int width, int height) : width(width), height(height) {
    d = XOpenDisplay(nullptr);
    if (!d) {
        std::cerr << "Cannot open display" << std::endl;
        exit(1);
    }

    s = DefaultScreen(d);
    w = XCreateSimpleWindow(d, RootWindow(d, s), 10, 10, width, height, 1,
                            BlackPixel(d, s), WhitePixel(d, s));
    XSelectInput(d, w, ExposureMask | KeyPressMask);
    XMapRaised(d, w);

    Pixmap pix = XCreatePixmap(d, w, width, height, DefaultDepth(d, DefaultScreen(d)));
    gc = XCreateGC(d, pix, 0, (XGCValues *)0);

    initColours();

    // Fix window size (non-resizable)
    XSizeHints hints;
    hints.flags = (USPosition | PSize | PMinSize | PMaxSize);
    hints.height = hints.base_height = hints.min_height = hints.max_height = height;
    hints.width = hints.base_width = hints.min_width = hints.max_width = width;
    XSetNormalHints(d, w, &hints);
    backBuffer = XCreatePixmap(d, w, width, height, DefaultDepth(d, DefaultScreen(d)));

    // XSynchronize(d, False); // let it do it async for speed :O
    usleep(1000);
}

void Xwindow::initColours() {
    colours.resize(BOARD_SIZE);

    colours[White] = WhitePixel(d, s);
    colours[Black] = BlackPixel(d, s);

    // other named colours
    const char *extraColours[] = {
        "red",        // index 2
        "green",      // 3
        "blue",       // 4
        "#EEE8AA",    // 5 LightBrown
        "#8B4513",    // 6 DarkBrown
        "grey"        // 7 Grey
    };

    Colormap cmap = DefaultColormap(d, DefaultScreen(d));
    XColor xcolour;

    for (int i = 0; i < 6; ++i) {
        if (XParseColor(d, cmap, extraColours[i], &xcolour) && 
            XAllocColor(d, cmap, &xcolour)) {
            colours[2 + i] = xcolour.pixel;
        } else {
            // fallback if parse fails
            colours[2 + i] = BlackPixel(d, s);
        }
    }
}

Xwindow::~Xwindow() {
    XFreeGC(d, gc);
    XCloseDisplay(d);
}

int Xwindow::getWidth() const { return width; }
int Xwindow::getHeight() const { return height; }

void Xwindow::fillRectangle(int x, int y, int width, int height, int colour) {
    if (colour < 0 || colour >= (int)colours.size()) colour = Black;
    XSetForeground(d, gc, colours[colour]);
    XFillRectangle(d, backBuffer, gc, x, y, width, height);
}

void Xwindow::drawString(int x, int y, const std::string &msg, int colour) {
    if (colour < 0 || colour >= (int)colours.size()) colour = Black;
    XSetForeground(d, gc, colours[colour]);
    XDrawString(d, backBuffer, gc, x, y, msg.c_str(), msg.length());
}

void Xwindow::flush() {
    XCopyArea(d, backBuffer, w, gc, 0, 0, width, height, 0, 0);
    XFlush(d);
}
