#ifndef __WINDOW_H__
#define __WINDOW_H__
#include <X11/Xlib.h>
#include <string>
#include <vector>

// X11 wrapper to make things easier to work with
class Xwindow {
    Display *d;
    Window w;
    int s, width, height;
    GC gc;
    Pixmap backBuffer;
    std::vector<unsigned long> colours;
    const int NUM_OF_COLOURS = 8;

public:
    Xwindow(int width = 500, int height = 500);  // Constructor
    ~Xwindow();                                  // Destructor

    // Basic colour enum
    enum Colour { White = 0, Black, Red, Green, Blue, LightBrown, DarkBrown, Grey };

    int getWidth() const;
    int getHeight() const;

    // Fill rectangle (background squares)
    void fillRectangle(int x, int y, int width, int height, int colour = Black);

    // Draw a string
    void drawString(int x, int y, const std::string &msg, int colour = Black);

    void flush();

private:
    void initColours(); // Helper to load all colours
};

#endif
