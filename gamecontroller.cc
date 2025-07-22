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


void GameController::handleSetupCommand(const std::string &action, std::istringstream &iss) {
    static Colour forcedTurn = turn;  // Keep last chosen turn

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
        std::cout << "Setup complete.";
    }
    else {
        std::cout << "Unknown setup command. Use +, -, =, done.\n";
    }
}


void GameController::handleNormalCommand(const std::string &action, std::istringstream &iss) {
    if (action == "game") {
        cmdGame(iss);
    }
    else if (action == "move") {
        cmdMove(iss);
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

    players[0] = whitePlayer;
    players[1] = blackPlayer;
    startGame(whitePlayer, blackPlayer);
}


void GameController::cmdMove(std::istringstream &iss) {
    if (!isGameRunning) {
        std::cout << "No game is currently running. Use 'game' to start.\n";
        return;
    }

    std::string fromSq, toSq;
    iss >> fromSq >> toSq;
    if (fromSq.empty() || toSq.empty()) {
        std::cout << "Usage: move <from> <to>\n";
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

    // Perform move
    if (!board->movePiece(from, to)) {
        std::cout << "Invalid move!\n";
        return;
    }
    board->notifyObservers();

    // Check for check/checkmate
    if (checkGameState()) {
        isGameRunning = false;
        return;
    }

    nextTurn();
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
