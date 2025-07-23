#include "gamecontroller.h"
#include "humanplayer.h"
#include "ailevel1.h"
#include <sstream>
#include <iostream>

// Helper: convert algebraic notation ("e2") → Position
static Position parseAlgebraic(const std::string &sq) {
    int col = sq[0] - 'a';
    int row = sq[1] - '1';
    return Position(row, col);
}

GameController::GameController() : turn(Colour::White), isGameRunning(false) {
    board = new Board();
    players[0] = nullptr;
    players[1] = nullptr;
}

GameController::~GameController() {
    delete board;
    // Note: players are NOT deleted, assumed external ownership
}

void GameController::attachDisplay(Display* d) {
    displays.push_back(d);
}

void GameController::setBoard(Board* newBoard) {
    if (board) delete board;
    board = newBoard;
}

Board& GameController::getBoard() {
    return *board;
}

Colour GameController::getCurrentTurn() const {
    return turn;
}

void GameController::startGame(Player* white, Player* black) {
    players[0] = white;
    players[1] = black;
    turn = Colour::White;
    board->resetBoard();
    isGameRunning = true;

    std::cout << "Game started! White moves first.\n";

    // Attach all displays as observers
    for (auto d : displays) {
        board->attach(d);
        d->notify(*board);
    }
}

Player* GameController::createPlayerFromString(const std::string& type, Colour c) {
    if (type == "human") {
        return new HumanPlayer(c);
    } else if (type.find("computer") == 0) {
        // :p
    }
    return nullptr;
}

void GameController::nextTurn() {
    turn = (turn == Colour::White) ? Colour::Black : Colour::White;
}

bool GameController::checkGameState() {
    if (board->isCheckMate(turn)) {
        std::cout << (turn == Colour::White ? "White" : "Black")
                  << " is checkmated! "
                  << (turn == Colour::White ? "Black" : "White")
                  << " wins!\n";
        return true;
    }
    else if (board->isInCheck(turn)) {
        std::cout << (turn == Colour::White ? "White" : "Black")
                  << " is in check!\n";
    }
    return false;
}

void GameController::processCommand(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;

    if (action.empty()) return;

    // Enter setup mode explicitly
    if (action == "setup") {
        inSetupMode = true;
        std::cout << "Entering setup mode. Use +, -, =, done.\n";
        return;
    }

    // Dispatch to correct handler
    if (inSetupMode)
        handleSetupCommand(action, iss);
    else
        handleNormalCommand(action, iss);
}

// =====================
// Setup mode commands
// =====================
void GameController::handleSetupCommand(const std::string &action, std::istringstream &iss) {
    static Colour forcedTurn = turn; // keep last chosen turn

    if (action == "+") {
        char pieceChar;
        std::string square;
        iss >> pieceChar >> square;

        if (square.empty()) {
            std::cout << "Usage: + <PieceChar> <square>\n";
            return;
        }

        Position pos = parseAlgebraic(square);
        board->placePiece(pieceChar, pos);
        board->notifyObservers();
    }
    else if (action == "-") {
        std::string square;
        iss >> square;
        if (square.empty()) {
            std::cout << "Usage: - <square>\n";
            return;
        }
        Position pos = parseAlgebraic(square);
        board->removePiece(pos);
        board->notifyObservers();
    }
    else if (action == "=") {
        std::string colourStr;
        iss >> colourStr;
        if (colourStr == "white") forcedTurn = Colour::White;
        else if (colourStr == "black") forcedTurn = Colour::Black;
        else {
            std::cout << "Invalid colour. Use white or black.\n";
            return;
        }
        std::cout << "Turn set to "
                  << (forcedTurn == Colour::White ? "white\n" : "black\n");
    }
    else if (action == "done") {
        if (!board->validateSetup()) {
            std::cout << "Invalid setup! Must have exactly 1 white king and 1 black king, "
                         "no pawns on back rank, and no king in check.\n";
            return;
        }
        turn = forcedTurn;
        inSetupMode = false;
        std::cout << "Setup complete.\n";
    }
    else {
        std::cout << "Unknown setup command. Use +, -, =, done.\n";
    }
}

// =====================
// Normal commands
// =====================
void GameController::handleNormalCommand(const std::string &action, std::istringstream &iss) {
    if (action == "game") {
        cmdGame(iss);
    }
    else if (action == "move") {
        if (!isGameRunning) {
            std::cout << "No game running. Use 'game' to start.\n";
            return;
        }

        Player* currentPlayer = (turn == Colour::White) ? players[0] : players[1];
        if (!currentPlayer) {
            std::cout << "No player for this colour.\n";
            return;
        }

        // Let the player handle the move 
        if (!currentPlayer->makeMove(*board, iss)) {
            std::cout << "Invalid move.\n";
            return;
        }

        board->notifyObservers();
        if (checkGameState()) {
            isGameRunning = false;
            return;
        }
        nextTurn();
    }
    else if (action == "resign") {
        cmdResign();
    }
    else {
        std::cout << "Unknown command.\n";
    }
}

void GameController::cmdGame(std::istringstream &iss) {
    std::string whitePlayerType, blackPlayerType;
    iss >> whitePlayerType >> blackPlayerType;

    if (whitePlayerType.empty() || blackPlayerType.empty()) {
        std::cout << "Usage: game white-player black-player\n";
        return;
    }

    Player* whitePlayer = createPlayerFromString(whitePlayerType, Colour::White);
    Player* blackPlayer = createPlayerFromString(blackPlayerType, Colour::Black);

    if (!whitePlayer || !blackPlayer) {
        std::cout << "Invalid player types.\n";
        return;
    }

    startGame(whitePlayer, blackPlayer);
}

void GameController::cmdResign() {
    if (!isGameRunning) {
        std::cout << "No game is currently running.\n";
        return;
    }
    std::cout << (turn == Colour::White ? "White" : "Black")
              << " resigns! "
              << (turn == Colour::White ? "Black" : "White")
              << " wins!\n";
    isGameRunning = false;
}
