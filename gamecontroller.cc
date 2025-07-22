#include "gamecontroller.h"
#include <sstream>
#include <iostream>


// helper: convert algebraic notation ("e2") to Position
static Position parseAlgebraic(const std::string &sq) {
    // 'a' -> 0, '1' -> 0
    int col = sq[0] - 'a';
    int row = sq[1] - '1';
    return Position(row, col);
}


GameController::GameController() : turn{Colour::White} {
    players[0] = nullptr;
    players[1] = nullptr;
}


GameController::~GameController() {
    // Don't delete players -> they might be stack-allocated elsewhere
    // Displays may also be owned externally
}


void GameController::startGame(Player* white, Player* black) {
    players[0] = white;
    players[1] = black;
    turn = Colour::White;
    board.resetBoard();

    std::cout << "Game started! White moves first.\n";

    // Attach all displays as observers
    for (auto d : displays) {
        board.attach(d);
        d->notify(board); // initial render
    }
}


void GameController::attachDisplay(Display* d) {
    displays.push_back(d);
}


Colour GameController::getCurrentTurn() const {
    return turn;
}


Player* GameController::getCurrentPlayer() {
    // turn == White -> players[0], turn == Black -> players[1]
    return (turn == Colour::White) ? players[0] : players[1];
}


Board &GameController::getBoard() {
    return board;
}


void GameController::nextTurn() {
    turn = (turn == Colour::White) ? Colour::Black : Colour::White;
}


// Process commands like "move e2 e4"
void GameController::processCommand(const std::string& cmd) {
    std::istringstream iss(cmd);
    std::string action;
    iss >> action;

    if (action == "move") {
        std::string fromSq, toSq;
        iss >> fromSq >> toSq;
        if (fromSq.empty() || toSq.empty()) {
            std::cout << "Invalid move format.\n";
            return;
        }

        Position from = parseAlgebraic(fromSq);
        Position to = parseAlgebraic(toSq);

        Piece* piece = board.getPieceAt(from);
        if (!piece) {
            std::cout << "No piece at " << fromSq << "\n";
            return;
        }
        if (piece->getColour() != turn) {
            std::cout << "It's not your turn!\n";
            return;
        }

        if (!board.validMove(from, to)) {
            std::cout << "Invalid move!\n";
            return;
        }

        // Perform the move
        board.movePiece(from, to);

        // Notify displays
        board.notifyObservers();

        // Check game state (check/checkmate)
        if (checkGameState()) return;

        // Next player's turn
        nextTurn();
    }
    else if (action == "resign") {
        std::cout << (turn == Colour::White ? "White" : "Black") << " resigns! "
                  << (turn == Colour::White ? "Black" : "White") << " wins!\n";
    }
    else {
        std::cout << "Unknown command.\n";
    }
}

bool GameController::checkGameState() {
    if (board.isCheckMate(turn)) {
        std::cout << (turn == Colour::White ? "White" : "Black") << " is checkmated! "
                  << (turn == Colour::White ? "Black" : "White") << " wins!\n";
        return true;
    }
    else if (board.isInCheck(turn)) {
        std::cout << (turn == Colour::White ? "White" : "Black") << " is in check!\n";
    }
    return false;
}
