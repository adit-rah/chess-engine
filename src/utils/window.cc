#include "window.h"
#include <iostream>
#include <cstdlib>

SDLWindow::SDLWindow(int width, int height) : width(width), height(height) {
    // Initialize SDL
    if (SDL_Init(SDL_INIT_VIDEO) < 0) {
        std::cerr << "SDL initialization failed: " << SDL_GetError() << std::endl;
        exit(1);
    }

    // Initialize SDL_ttf for text rendering
    if (TTF_Init() < 0) {
        std::cerr << "SDL_ttf initialization failed: " << TTF_GetError() << std::endl;
        SDL_Quit();
        exit(1);
    }

    // Create window
    window = SDL_CreateWindow(
        "Chess Game",
        SDL_WINDOWPOS_CENTERED,
        SDL_WINDOWPOS_CENTERED,
        width,
        height,
        SDL_WINDOW_SHOWN
    );

    if (!window) {
        std::cerr << "Window creation failed: " << SDL_GetError() << std::endl;
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }

    // Create renderer with hardware acceleration
    renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED | SDL_RENDERER_PRESENTVSYNC);
    if (!renderer) {
        std::cerr << "Renderer creation failed: " << SDL_GetError() << std::endl;
        SDL_DestroyWindow(window);
        TTF_Quit();
        SDL_Quit();
        exit(1);
    }

    // Load font (try multiple common font paths)
    const char* fontPaths[] = {
        "/usr/share/fonts/truetype/dejavu/DejaVuSans-Bold.ttf",
        "/usr/share/fonts/TTF/DejaVuSans-Bold.ttf",
        "C:\\Windows\\Fonts\\arial.ttf",
        "/System/Library/Fonts/Helvetica.ttc",
        nullptr
    };

    font = nullptr;
    for (int i = 0; fontPaths[i] != nullptr; ++i) {
        font = TTF_OpenFont(fontPaths[i], 48);
        if (font) break;
    }

    if (!font) {
        std::cerr << "Warning: Could not load font. Text rendering may not work properly." << std::endl;
        std::cerr << "TTF_Error: " << TTF_GetError() << std::endl;
    }

    initColours();
    clear();
}

void SDLWindow::initColours() {
    colours.resize(NUM_OF_COLOURS);

    colours[White] = {255, 255, 255, 255};
    colours[Black] = {0, 0, 0, 255};
    colours[Red] = {255, 0, 0, 255};
    colours[Green] = {0, 255, 0, 255};
    colours[Blue] = {0, 0, 255, 255};
    colours[LightBrown] = {238, 232, 170, 255};  // #EEE8AA
    colours[DarkBrown] = {139, 69, 19, 255};     // #8B4513
    colours[Grey] = {128, 128, 128, 255};
}

SDLWindow::~SDLWindow() {
    if (font) TTF_CloseFont(font);
    if (renderer) SDL_DestroyRenderer(renderer);
    if (window) SDL_DestroyWindow(window);
    TTF_Quit();
    SDL_Quit();
}

int SDLWindow::getWidth() const { return width; }
int SDLWindow::getHeight() const { return height; }

void SDLWindow::setDrawColor(int colour) {
    if (colour < 0 || colour >= (int)colours.size()) colour = Black;
    const Color& c = colours[colour];
    SDL_SetRenderDrawColor(renderer, c.r, c.g, c.b, c.a);
}

void SDLWindow::fillRectangle(int x, int y, int w, int h, int colour) {
    setDrawColor(colour);
    SDL_Rect rect = {x, y, w, h};
    SDL_RenderFillRect(renderer, &rect);
}

void SDLWindow::drawString(int x, int y, const std::string &msg, int colour) {
    if (!font || msg.empty()) return;

    const Color& c = colours[colour];
    SDL_Color sdlColor = {c.r, c.g, c.b, c.a};

    // Render text to surface
    SDL_Surface* surface = TTF_RenderText_Blended(font, msg.c_str(), sdlColor);
    if (!surface) {
        std::cerr << "Text rendering failed: " << TTF_GetError() << std::endl;
        return;
    }

    // Create texture from surface
    SDL_Texture* texture = SDL_CreateTextureFromSurface(renderer, surface);
    if (!texture) {
        SDL_FreeSurface(surface);
        return;
    }

    // Get text dimensions and create destination rectangle
    SDL_Rect destRect = {x, y - surface->h / 2, surface->w, surface->h};

    // Render texture
    SDL_RenderCopy(renderer, texture, nullptr, &destRect);

    // Cleanup
    SDL_DestroyTexture(texture);
    SDL_FreeSurface(surface);
}

void SDLWindow::clear() {
    SDL_SetRenderDrawColor(renderer, 255, 255, 255, 255);
    SDL_RenderClear(renderer);
}

void SDLWindow::flush() {
    SDL_RenderPresent(renderer);
}
