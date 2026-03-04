#include "gamecontroller.h"
#include "humanplayer.h"
#include "ailevel1.h"
#include "ailevel2.h"
#include "ailevel3.h"
#include "ailevel4.h"
#include <sstream>
#include <iostream>

// Helper: convert algebraic notation ("e2") → Position
static Position parseAlgebraic(const std::string &sq) {
    int col = sq[0] - 'a';
    int row = sq[1] - '1';
    return Position(row, col);
}

GameController::GameController() : turn(Colour::White), isGameRunning(false) {
    players[0] = nullptr;
    players[1] = nullptr;
    board = nullptr; // this is handled elsewhere
}

GameController::~GameController() {
    delete board;
    delete players[0];
    delete players[1];
}

void GameController::attachDisplay(AbstractDisplay* d) {
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
    if (!board) {
        board = new Board();
        board->resetBoard();
    }
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
    } else if (type == "computer1") {
        return new AILevel1(c); 
    } else if (type == "computer2") {
        return new AILevel2(c); 
    } else if (type == "computer3") {
        return new AILevel3(c); 
    } else if (type == "computer4") {
        return new AILevel4(c); 
    }
    return nullptr;
}


void GameController::resetGame() {
    // free existing players
    delete players[0];
    delete players[1];
    players[0] = nullptr;
    players[1] = nullptr;

    // reset the turn
    turn = Colour::White;
    isGameRunning = false;

    if (board) {
        delete board; 
        board = nullptr;    // dangling!!
    }

    std::cout << "Game has been reset.\n";
}


void GameController::nextTurn() {
    turn = (turn == Colour::White) ? Colour::Black : Colour::White;
}

bool GameController::checkGameState() {
    Colour opp = (turn == Colour::White) ? Colour::Black : Colour::White;
    // you need to see if the opponent is in checkmate after a move
    if (board->isCheckMate(opp)) {
        std::cout << (opp == Colour::White ? "White" : "Black")
                  << " is checkmated! "
                  << (opp == Colour::White ? "Black" : "White")
                  << " wins!\n";

        if (turn == Colour::White) whiteScore += 1.0f;
        else blackScore += 1.0f;
        
        return true;
    }
    // you need to see if the opponent is in check after a move 
    else if (board->isInCheck(opp)) {
        std::cout << (opp == Colour::White ? "White" : "Black")
                  << " is in check!\n";
    }
    // you can then check stalemate
    else if (board->isStaleMate(opp)) {
        std::cout << "Stalemate! The game is a draw.\n";

        whiteScore += 0.5f;
        blackScore += 0.5f;

        return true;
    }
    else if (board->insufficientMaterial()) {
        std::cout << "Draw by insufficient material.\n";

        whiteScore += 0.5f;
        blackScore += 0.5f;

        return true;
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
        if (!board) {
            board = new Board();
            board->emptyTheBoard();
        }
        return;
    }

    // Dispatch to correct handler
    if (inSetupMode)
        handleSetupCommand(action, iss);
    else
        handleNormalCommand(action, iss);
}

// Setup mode commands
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

// Normal commands
void GameController::handleNormalCommand(const std::string &action, std::istringstream &iss) {
    if (action == "game") cmdGame(iss);
    else if (action == "move") cmdMove(iss);
    else if (action == "resign") cmdResign();
    else if (action == "autoplay") cmdAutoplay();
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


void GameController::cmdMove(std::istringstream& iss) {
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
        resetGame(); 
        return;
    }
    nextTurn();
}

void GameController::cmdAutoplay() {
    if (!isGameRunning) {
        std::cout << "No game running. Use 'game' to start.\n";
        return;
    }

    std::cout << "Autoplay running...\n";

    while (isGameRunning) {
        Player* currentPlayer = (turn == Colour::White) ? players[0] : players[1];

        // Try a move
        std::istringstream dummy;
        if (!currentPlayer->makeMove(*board, dummy)) break;

        // Print last move
        auto from = board->getLastMoveFrom();
        auto to   = board->getLastMoveTo();
        std::cout << (turn == Colour::White ? "White" : "Black") 
                  << " moves " << char('a' + from.col) << (from.row + 1)
                  << " -> "   << char('a' + to.col)   << (to.row + 1) << "\n";

        board->notifyObservers();

        if (checkGameState()) {
            isGameRunning = false; 
            resetGame();
            break;
        }
        nextTurn();
    }

    std::cout << "Autoplay finished.\n";
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
    
    if (turn == Colour::White) blackScore += 1.0f;
    else whiteScore += 1.0f;

    isGameRunning = false;
    resetGame();
}

void GameController::printFinalScore() const {
    std::cout << "Final Score:\n";
    std::cout << "White: " << whiteScore << "\n";
    std::cout << "Black: " << blackScore << "\n";
}

bool GameController::tryHumanMove(const std::string& from, const std::string& to) {
    if (!isGameRunning) return false;
    Player* currentPlayer = (turn == Colour::White) ? players[0] : players[1];
    if (!currentPlayer) return false;
    if (dynamic_cast<HumanPlayer*>(currentPlayer) == nullptr) return false;
    std::istringstream iss("move " + from + " " + to);
    std::string action;
    iss >> action;
    if (action != "move") return false;
    if (!currentPlayer->makeMove(*board, iss)) return false;
    board->notifyObservers();
    if (checkGameState()) {
        isGameRunning = false;
    } else {
        nextTurn();
    }
    return true;
}

std::string GameController::doComputerMove() {
    if (!isGameRunning) return "";
    Player* currentPlayer = (turn == Colour::White) ? players[0] : players[1];
    if (!currentPlayer) return "";
    std::istringstream dummy;
    if (!currentPlayer->makeMove(*board, dummy)) return "";
    auto from = board->getLastMoveFrom();
    auto to = board->getLastMoveTo();
    std::string result;
    result += char('a' + from.col);
    result += char('1' + from.row);
    result += char('a' + to.col);
    result += char('1' + to.row);
    board->notifyObservers();
    if (checkGameState()) {
        isGameRunning = false;
    } else {
        nextTurn();
    }
    return result;
}

