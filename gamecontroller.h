#ifndef GAMECONTROLLER_H
#define GAMECONTROLLER_H

#include "board.h"
#include "player.h"
#include "display.h"
#include <memory>
#include <string>

class GameController {
    Board *board;                       // Owns the chess board
    Player *players[2];                 // Two players (could be Human or Computer)
    Colour turn;                        // Whose turn is it? White/Black
    std::vector<AbstractDisplay*> displays;     // Attached displays (Text/Graphical)
    
    bool isGameRunning = false;         // flags
    bool inSetupMode = false;

    // Command Handling
    void handleSetupCommand(const std::string &action, std::istringstream &iss);
    void handleNormalCommand(const std::string &action, std::istringstream &iss);
    void cmdGame(std::istringstream &iss);
    void cmdResign();

public:
    GameController();
    ~GameController();

    // Core setup
    void startGame(Player* white, Player* black);  // Initialize board, players
    void attachDisplay(AbstractDisplay* d);                // Add a display observer
    void setBoard(Board* newBoard);
    Player* createPlayerFromString(const std::string& type, Colour c);

    // Main game loop helpers
    void processCommand(const std::string &cmd);
    void nextTurn();                               // Switch to the other player
    bool checkGameState();                         // Detect check, checkmate, stalemate;

    // Utility
    Colour getCurrentTurn() const;
    Board& getBoard();
};

#endif
