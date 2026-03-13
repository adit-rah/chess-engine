#include "gamecontroller.h"
#include "textdisplay.h"
#include "graphicsdisplay.h"
#include "logdisplay.h"

#include <functional>
#include <iostream>
#include <string>

#if defined(__unix__) || defined(__APPLE__)
#include <unistd.h>
#include <sys/select.h>
#endif
#include <SDL.h>

namespace {

// Reads a line from stdin. When idle() is non-null, calls it periodically while
// waiting (so e.g. SDL can pump events). idle() returns true to request quit.
// Returns false on EOF or when idle() requested quit.
bool readLineWithIdle(std::string& line, std::function<bool()> idle) {
    if (!idle) {
        return static_cast<bool>(std::getline(std::cin, line));
    }
#if defined(__unix__) || defined(__APPLE__)
    for (;;) {
        fd_set fds;
        FD_ZERO(&fds);
        FD_SET(STDIN_FILENO, &fds);
        struct timeval tv = {0, 50000};
        if (select(STDIN_FILENO + 1, &fds, nullptr, nullptr, &tv) > 0 &&
            FD_ISSET(STDIN_FILENO, &fds))
            break;
        if (idle()) return false;
    }
#endif
    return static_cast<bool>(std::getline(std::cin, line));
}

}  // namespace

int main(int argc, char *argv[]) {
    GameController game;
    TextDisplay textDisp;
    game.attachDisplay(&textDisp);

    bool enableGraphics = false;
    bool enableLogging = false;

    // check for the flags
    for (int i = 1; i < argc; ++i) {
        if (std::string(argv[i]) == "-g" || std::string(argv[i]) == "-graphics") {
            enableGraphics = true;
        }
        if (std::string(argv[i]) == "-l" || std::string(argv[i]) == "-log") {
            enableLogging = true;
        }
    }

    std::unique_ptr<GraphicsDisplay> gfxDisp;
    if (enableGraphics) {
        gfxDisp = std::make_unique<GraphicsDisplay>();  // only allocate if needed
        game.attachDisplay(gfxDisp.get());
    }

    std::unique_ptr<LogDisplay> log;
    if (enableLogging) {
        log = std::make_unique<LogDisplay>();           // only allocate if needed
        game.attachDisplay(log.get());
    }

    std::cout << "Welcome to Chess!\n";
    std::cout << "Commands:\n";
    std::cout << "  game white-player black-player   (human or computer[1-5])\n";
    std::cout << "  move e2 e4                       (or just 'move' for AI)\n";
    std::cout << "  autoplay                         (two computer players are required)\n";
    std::cout << "  resign\n";
    std::cout << "  setup (then +K e1, -e1, etc.)\n";
    std::cout << "  Ctrl+D to quit.\n\n";

    std::string cmd;
    auto pumpSdl = enableGraphics ? std::function<bool()>{[] {
        SDL_PumpEvents();
        SDL_Event e;
        while (SDL_PollEvent(&e))
            if (e.type == SDL_QUIT) return true;
        return false;
    }} : std::function<bool()>{};

    while (true) {
        std::cout << "> " << std::flush;
        if (!readLineWithIdle(cmd, pumpSdl)) break;
        if (cmd.empty()) continue;
        if (cmd == "quit" || cmd == "exit") break;
        game.processCommand(cmd);
    }

    game.printFinalScore();
    std::cout << "Goodbye!\n";
    return 0;
}
