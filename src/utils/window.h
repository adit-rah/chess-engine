#ifndef __WINDOW_H__
#define __WINDOW_H__
#include <SDL2/SDL.h>
#include <SDL2/SDL_ttf.h>
#include <string>
#include <vector>
#include <memory>

// SDL2 wrapper for graphics rendering
class SDLWindow {
    SDL_Window* window;
    SDL_Renderer* renderer;
    TTF_Font* font;
    int width, height;
    
    struct Color {
        Uint8 r, g, b, a;
    };
    std::vector<Color> colours;
    const int NUM_OF_COLOURS = 8;

public:
    SDLWindow(int width = 500, int height = 500);  // Constructor
    ~SDLWindow();                                  // Destructor

    // Basic colour enum (matching old API for compatibility)
    enum Colour { White = 0, Black, Red, Green, Blue, LightBrown, DarkBrown, Grey };

    int getWidth() const;
    int getHeight() const;

    // Fill rectangle (background squares)
    void fillRectangle(int x, int y, int width, int height, int colour = Black);

    // Draw a string
    void drawString(int x, int y, const std::string &msg, int colour = Black);

    void flush();
    void clear();

private:
    void initColours(); // Helper to load all colours
    void setDrawColor(int colour);
};

// Type alias for backward compatibility
using Xwindow = SDLWindow;

#endif
