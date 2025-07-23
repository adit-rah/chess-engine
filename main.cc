#include "gamecontroller.h"
#include "textdisplay.h"
#include "player.h"
#include <iostream>
#include <string>

int main() {
    GameController game;
    TextDisplay textDisp;
    game.attachDisplay(&textDisp);

    std::cout << "Welcome to Chess!\n";
    std::cout << "Commands:\n";
    std::cout << "  game white-player black-player   (human or computer1)\n";
    std::cout << "  move e2 e4                      (or just 'move' for AI)\n";
    std::cout << "  resign\n";
    std::cout << "  setup (then +K e1, -e1, etc.)\n";
    std::cout << "  Ctrl+D to quit.\n\n";

    std::string cmd;
    while (true) {
        std::cout << "> ";
        std::string cmd;
        if (!std::getline(std::cin, cmd)) break; // EOF exits
        if (cmd.empty()) continue;               // <-- ignore empty lines

        if (cmd == "quit" || cmd == "exit") break;
        game.processCommand(cmd);
    }

    std::cout << "Goodbye!\n";
    return 0;
}
