#include "gamecontroller.h"
#include "humanplayer.h"
#include "ailevel1.h"
#include <sstream>
#include <iostream>


// helper: convert algebraic notation ("e2") to Position
static Position parseAlgebraic(const std::string &sq) {
    // 'a' -> 0, '1' -> 0
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
    // Don't delete players -> they might be stack-allocated elsewhere
    // Displays may also be owned externally
    delete board;
}


void GameController::startGame(Player* white, Player* black) {
    players[0] = white;     // player0 is white
    players[1] = black;     // player1 is black
    turn = Colour::White;   // start with white
    board->resetBoard();    // clean up board
    isGameRunning = true;   // start the game

    std::cout << "Game started! White moves first.\n";

    // Attach all displays as observers
    for (auto d : displays) {
        board->attach(d);
        d->notify(*board); // initial render
    }
}


void GameController::attachDisplay(Display* d) {
    displays.push_back(d);
}


void GameController::setBoard(Board* newBoard) {
    if (board) delete board;  // clean up old board
    board = newBoard;
}


Colour GameController::getCurrentTurn() const {
    return turn;
}


Player* GameController::getCurrentPlayer() {
    // turn == White -> players[0], turn == Black -> players[1]
    return (turn == Colour::White) ? players[0] : players[1];
}


Board &GameController::getBoard() {
    return *board;
}


void GameController::nextTurn() {
    turn = (turn == Colour::White) ? Colour::Black : Colour::White;
}


Player* GameController::createPlayerFromString(const std::string& type, Colour c) {
    if (type == "human") {
        return new HumanPlayer(c);
    } else if (type.find("computer") == 0) {
        // For now, all computer types map to AILevel1; extend as needed
    }
    return nullptr;
}


void GameController::processCommand(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;

    if (action == "game") {
        // Start a new game: e.g. "game human computer1"
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

        // Clean up old players if owned here (optional)
        players[0] = whitePlayer;
        players[1] = blackPlayer;

        startGame(whitePlayer, blackPlayer);
    }
    else if (action == "move") {
        if (!isGameRunning) {
            std::cout << "No game is currently running. Use 'game' command to start.\n";
            return;
        }

        std::string fromSq, toSq;
        iss >> fromSq >> toSq;
        if (fromSq.empty() || toSq.empty()) {
            std::cout << "Invalid move format.\n";
            return;
        }

        Position from = parseAlgebraic(fromSq);
        Position to = parseAlgebraic(toSq);

        Piece* piece = board->getPieceAt(from);
        if (!piece) {
            std::cout << "No piece at " << fromSq << "\n";
            return;
        }
        if (piece->getColour() != turn) {
            std::cout << "It's not your turn!\n";
            return;
        }

        if (!board->validMove(from, to)) {
            std::cout << "Invalid move!\n";
            return;
        }

        // Perform the move
        board->movePiece(from, to);

        // Notify displays
        board->notifyObservers();

        // Check game state (check/checkmate)
        if (checkGameState()) {
            isGameRunning = false;
            return;
        }

        // Next player's turn
        nextTurn();
    }
    else if (action == "resign") {
        if (!isGameRunning) {
            std::cout << "No game is currently running.\n";
            return;
        }
        std::cout << (turn == Colour::White ? "White" : "Black") << " resigns! "
                  << (turn == Colour::White ? "Black" : "White") << " wins!\n";
        isGameRunning = false;
    }
    else {
        std::cout << "Unknown command.\n";
    }
}

bool GameController::checkGameState() {
    if (board->isCheckMate(turn)) {
        std::cout << (turn == Colour::White ? "White" : "Black") << " is checkmated! "
                  << (turn == Colour::White ? "Black" : "White") << " wins!\n";
        return true;
    }
    else if (board->isInCheck(turn)) {
        std::cout << (turn == Colour::White ? "White" : "Black") << " is in check!\n";
    }
    return false;
}
