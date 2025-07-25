#include "gamecontroller.h"
#include "textdisplay.h"

#ifdef GRAPHICS
#include "graphicsdisplay.h"
#endif

#include <iostream>
#include <string>

int main() {
    GameController game;
    TextDisplay textDisp;
    game.attachDisplay(&textDisp);

#ifdef GRAPHICS
    GraphicsDisplay gfxDisp;
    game.attachDisplay(&gfxDisp);
    std::cout << "Enabling Graphics Mode\n";
#endif

    std::cout << "Welcome to Chess!\n";
    std::cout << "Commands:\n";
    std::cout << "  game white-player black-player      (human or computer[1-4])\n";
    std::cout << "  move e2 e4                          (or just 'move' for AI)\n";
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
