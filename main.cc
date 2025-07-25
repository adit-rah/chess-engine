#include "gamecontroller.h"
#include "textdisplay.h"
#include "graphicsdisplay.h"
#include "logdisplay.h"

#include <iostream>
#include <string>

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
    std::cout << "  game white-player black-player   (human or computer[1-4])\n";
    std::cout << "  move e2 e4                       (or just 'move' for AI)\n";
    std::cout << "  autoplay                         (two computer players are required)\n";
    std::cout << "  resign\n";
    std::cout << "  setup (then +K e1, -e1, etc.)\n";
    std::cout << "  Ctrl+D to quit.\n\n";

    std::string cmd;
    while (true) {
        std::cout << "> ";
        if (!std::getline(std::cin, cmd)) break;
        if (cmd.empty()) continue;

        if (cmd == "quit" || cmd == "exit") break;
        game.processCommand(cmd);
    }

    std::cout << "Goodbye!\n";
    return 0;
}
