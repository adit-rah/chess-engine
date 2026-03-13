#include "gamecontroller.h"
#include <emscripten/bind.h>
#include <string>
#include <sstream>
#include <vector>
static GameController* g_game = nullptr;

static bool isValidSquare(const std::string& sq) {
    return sq.size() == 2 && sq[0] >= 'a' && sq[0] <= 'h' && sq[1] >= '1' && sq[1] <= '8';
}

static bool isValidPlayer(const std::string& s) {
    return s == "human" || s == "computer1" || s == "computer2" || s == "computer3" || s == "computer4" || s == "computer5";
}

static std::string boardToFen(const Board& board) {
    std::string fen;
    for (int r = 7; r >= 0; --r) {
        int empty = 0;
        for (int c = 0; c < 8; ++c) {
            Piece* p = board.getPieceAt(Position(r, c));
            if (!p || p->getType() == PieceType::None) {
                ++empty;
            } else {
                if (empty) { fen += std::to_string(empty); empty = 0; }
                char sym = p->getSymbol();
                if (p->getColour() == Colour::Black) sym = std::tolower(sym);
                fen += sym;
            }
        }
        if (empty) fen += std::to_string(empty);
        if (r > 0) fen += '/';
    }
    return fen;
}

void chess_init(const std::string& white, const std::string& black) {
    if (!isValidPlayer(white) || !isValidPlayer(black)) return;
    if (g_game) delete g_game;
    g_game = new GameController();
    std::string cmd = "game " + white + " " + black;
    g_game->processCommand(cmd);
}

bool chess_human_move(const std::string& from, const std::string& to) {
    if (!g_game || !isValidSquare(from) || !isValidSquare(to)) return false;
    return g_game->tryHumanMove(from, to);
}

std::string chess_computer_move() {
    if (!g_game) return "";
    return g_game->doComputerMove();
}

std::string chess_get_board() {
    if (!g_game) return "";
    try {
        return boardToFen(g_game->getBoard());
    } catch (...) {
        return "";
    }
}

std::string chess_get_turn() {
    if (!g_game || !g_game->getIsGameRunning()) return "";
    return g_game->getCurrentTurn() == Colour::White ? "white" : "black";
}

bool chess_is_running() {
    return g_game && g_game->getIsGameRunning();
}

std::string chess_get_move_history() {
    if (!g_game) return "[]";
    const auto& hist = g_game->getMoveHistory();
    std::string out = "[";
    for (size_t i = 0; i < hist.size(); ++i) {
        if (i > 0) out += ",";
        out += "\"" + hist[i] + "\"";
    }
    out += "]";
    return out;
}

void chess_reset() {
    if (g_game) {
        delete g_game;
        g_game = nullptr;
    }
}

void chess_process_command(const std::string& cmd) {
    if (cmd.empty()) return;
    if (!g_game) g_game = new GameController();
    g_game->processCommand(cmd);
}

void chess_resign() {
    if (g_game) g_game->processCommand("resign");
}

bool chess_is_setup_mode() {
    return g_game && g_game->getInSetupMode();
}

EMSCRIPTEN_BINDINGS(chess) {
    emscripten::function("init", &chess_init);
    emscripten::function("humanMove", &chess_human_move);
    emscripten::function("computerMove", &chess_computer_move);
    emscripten::function("getBoard", &chess_get_board);
    emscripten::function("getTurn", &chess_get_turn);
    emscripten::function("isRunning", &chess_is_running);
    emscripten::function("getMoveHistory", &chess_get_move_history);
    emscripten::function("processCommand", &chess_process_command);
    emscripten::function("resign", &chess_resign);
    emscripten::function("isSetupMode", &chess_is_setup_mode);
    emscripten::function("reset", &chess_reset);
}

int main() { return 0; }
