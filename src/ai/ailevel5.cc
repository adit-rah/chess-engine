#include "ailevel5.h"
#include "board.h"
#include "piece.h"
#include <vector>
#include <algorithm>
#include <utility>

namespace {

// Reduced from 4/4 for responsiveness; mobility removed (was O(pieces*moves) per eval)
const int SEARCH_DEPTH = 3;
const int QUIESCE_DEPTH = 2;
const int CHECKMATE_SCORE = 100000;
const int KING_DANGER_PENALTY = 50;

// Piece-square tables (from White's perspective; row 0 = white back rank)
// Values encourage center control, piece development, etc.
static const int PAWN_PST[8][8] = {
    { 0,  0,  0,  0,  0,  0,  0,  0},
    { 5, 10, 10,-20,-20, 10, 10,  5},
    { 5, -5,-10,  0,  0,-10, -5,  5},
    { 0,  0,  0, 20, 20,  0,  0,  0},
    { 5,  5, 10, 25, 25, 10,  5,  5},
    {10, 10, 20, 30, 30, 20, 10, 10},
    {50, 50, 50, 50, 50, 50, 50, 50},
    { 0,  0,  0,  0,  0,  0,  0,  0}
};

static const int KNIGHT_PST[8][8] = {
    {-50,-40,-30,-30,-30,-30,-40,-50},
    {-40,-20,  0,  0,  0,  0,-20,-40},
    {-30,  0, 10, 15, 15, 10,  0,-30},
    {-30,  5, 15, 20, 20, 15,  5,-30},
    {-30,  0, 15, 20, 20, 15,  0,-30},
    {-30,  5, 10, 15, 15, 10,  5,-30},
    {-40,-20,  0,  5,  5,  0,-20,-40},
    {-50,-40,-30,-30,-30,-30,-40,-50}
};

static const int BISHOP_PST[8][8] = {
    {-20,-10,-10,-10,-10,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5, 10, 10,  5,  0,-10},
    {-10,  5,  5, 10, 10,  5,  5,-10},
    {-10,  0, 10, 10, 10, 10,  0,-10},
    {-10, 10, 10, 10, 10, 10, 10,-10},
    {-10,  5,  0,  0,  0,  0,  5,-10},
    {-20,-10,-10,-10,-10,-10,-10,-20}
};

static const int ROOK_PST[8][8] = {
    {  0,  0,  0,  0,  0,  0,  0,  0},
    {  5, 10, 10, 10, 10, 10, 10,  5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    { -5,  0,  0,  0,  0,  0,  0, -5},
    {  0,  0,  0,  5,  5,  0,  0,  0}  // 7th rank bonus
};

static const int QUEEN_PST[8][8] = {
    {-20,-10,-10, -5, -5,-10,-10,-20},
    {-10,  0,  0,  0,  0,  0,  0,-10},
    {-10,  0,  5,  5,  5,  5,  0,-10},
    { -5,  0,  5,  5,  5,  5,  0, -5},
    {  0,  0,  5,  5,  5,  5,  0, -5},
    {-10,  5,  5,  5,  5,  5,  0,-10},
    {-10,  0,  5,  0,  0,  0,  0,-10},
    {-20,-10,-10, -5, -5,-10,-10,-20}
};

static const int KING_MID_PST[8][8] = {
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-30,-40,-40,-50,-50,-40,-40,-30},
    {-20,-30,-30,-40,-40,-30,-30,-20},
    {-10,-20,-20,-20,-20,-20,-20,-10},
    { 20, 20,  0,  0,  0,  0, 20, 20},
    { 20, 30, 10,  0,  0, 10, 30, 20}
};

struct ScoredMove {
    Position from;
    Position to;
    int orderScore;  // for move ordering (higher = try first)
};

int pstValue(Piece* p, int row, int col) {
    if (!p || p->getType() == PieceType::None) return 0;
    int r = (p->getColour() == Colour::White) ? row : (7 - row);
    int c = col;
    switch (p->getType()) {
        case PieceType::Pawn:   return PAWN_PST[r][c];
        case PieceType::Knight: return KNIGHT_PST[r][c];
        case PieceType::Bishop: return BISHOP_PST[r][c];
        case PieceType::Rook:   return ROOK_PST[r][c];
        case PieceType::Queen:  return QUEEN_PST[r][c];
        case PieceType::King:   return KING_MID_PST[r][c];
        default: return 0;
    }
}

// Static evaluation from perspective of colour (positive = good for colour)
// Mobility removed: was O(pieces * moves) per call, called at every leaf - too expensive
int staticEval(Board& b, Colour colour) {
    int material = 0;
    int pst = 0;
    Colour opp = (colour == Colour::White) ? Colour::Black : Colour::White;

    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = b.getPieceAt(Position(r, c));
            if (!p || p->getType() == PieceType::None) continue;

            int val = p->getValue();
            int posVal = pstValue(p, r, c);

            if (p->getColour() == colour) {
                material += val;
                pst += posVal;
            } else if (p->getColour() == opp) {
                material -= val;
                pst -= posVal;
            }
        }
    }

    int kingSafety = 0;
    if (b.isInCheck(colour)) kingSafety -= KING_DANGER_PENALTY;
    if (b.isInCheck(opp))    kingSafety += KING_DANGER_PENALTY;

    return material + pst + kingSafety;
}

void collectMoves(Board& b, Colour c, std::vector<ScoredMove>& out) {
    out.clear();
    out.reserve(48);  // typical max ~35 legal moves
    for (int r = 0; r < 8; ++r) {
        for (int col = 0; col < 8; ++col) {
            Position from(r, col);
            Piece* piece = b.getPieceAt(from);
            if (!piece || piece->getColour() != c) continue;

            std::vector<Position> moves = piece->getValidMoves(b);
            for (Position to : moves) {
                Piece* target = b.getPieceAt(to);
                int orderScore = 0;
                if (target && target->getColour() != c && target->getColour() != Colour::None) {
                    orderScore = 10000 + target->getValue();  // captures first, by value
                }
                // Non-captures get 0
                out.push_back({from, to, orderScore});
            }
        }
    }
    std::sort(out.begin(), out.end(), [](const ScoredMove& a, const ScoredMove& b) {
        return a.orderScore > b.orderScore;
    });
}

// Uses getRawMoves + filter for captures; faster than getValidMoves (avoids legality check for non-captures)
void collectCaptureMoves(Board& b, Colour c, std::vector<ScoredMove>& out) {
    out.clear();
    out.reserve(16);
    for (int r = 0; r < 8; ++r) {
        for (int col = 0; col < 8; ++col) {
            Position from(r, col);
            Piece* piece = b.getPieceAt(from);
            if (!piece || piece->getColour() != c) continue;

            std::vector<Position> raw = piece->getRawMoves(b);
            for (Position to : raw) {
                Piece* target = b.getPieceAt(to);
                if (!target || target->getColour() == c || target->getColour() == Colour::None) continue;
                if (!b.isBoardLegalMove(from, to)) continue;
                out.push_back({from, to, target->getValue()});
            }
        }
    }
    std::sort(out.begin(), out.end(), [](const ScoredMove& a, const ScoredMove& b) {
        return a.orderScore > b.orderScore;
    });
}

// Quiescence: capture-only search to avoid horizon effect
int quiescence(Board& b, Colour colourToMove, int alpha, int beta, int qDepth) {
    if (qDepth <= 0) return staticEval(b, colourToMove);

    Colour opp = (colourToMove == Colour::White) ? Colour::Black : Colour::White;

    std::vector<ScoredMove> captures;
    collectCaptureMoves(b, colourToMove, captures);

    if (captures.empty()) return staticEval(b, colourToMove);

    int best = staticEval(b, colourToMove);
    for (const ScoredMove& m : captures) {
        Board copy(b);
        if (!copy.movePiece(m.from, m.to)) continue;

        int score = -quiescence(copy, opp, -beta, -alpha, qDepth - 1);
        if (score > best) best = score;
        if (score > alpha) alpha = score;
        if (alpha >= beta) break;
    }
    return best;
}

// Entry point that uses quiescence at leaf nodes
int alphabetaWithQuiescence(Board& b, Colour colourToMove, int depth, int alpha, int beta) {
    Colour opp = (colourToMove == Colour::White) ? Colour::Black : Colour::White;

    if (b.isCheckMate(colourToMove)) return -CHECKMATE_SCORE;
    if (b.isCheckMate(opp)) return CHECKMATE_SCORE;
    if (b.isStaleMate(colourToMove)) return 0;

    if (depth == 0) {
        return quiescence(b, colourToMove, alpha, beta, QUIESCE_DEPTH);
    }

    std::vector<ScoredMove> moves;
    collectMoves(b, colourToMove, moves);

    if (moves.empty()) return staticEval(b, colourToMove);

    int best = -CHECKMATE_SCORE - 1;
    for (const ScoredMove& m : moves) {
        Board copy(b);
        if (!copy.movePiece(m.from, m.to)) continue;

        int score = -alphabetaWithQuiescence(copy, opp, depth - 1, -beta, -alpha);
        if (score > best) best = score;
        if (score > alpha) alpha = score;
        if (alpha >= beta) break;
    }
    return best;
}

}  // namespace

std::vector<Position> AILevel5::determineNextBestMove(Board& b) {
    std::vector<ScoredMove> moves;
    collectMoves(b, colour, moves);

    if (moves.empty()) return {Position(-1, -1), Position(-1, -1)};

    Colour opp = (colour == Colour::White) ? Colour::Black : Colour::White;
    int bestScore = -CHECKMATE_SCORE - 1;
    std::vector<std::pair<Position, Position>> bestMoves;

    for (const ScoredMove& m : moves) {
        Board copy(b);
        if (!copy.movePiece(m.from, m.to)) continue;

        int score = -alphabetaWithQuiescence(copy, opp, SEARCH_DEPTH - 1, -CHECKMATE_SCORE - 1, CHECKMATE_SCORE + 1);

        if (score > bestScore) {
            bestScore = score;
            bestMoves.clear();
            bestMoves.emplace_back(m.from, m.to);
        } else if (score == bestScore) {
            bestMoves.emplace_back(m.from, m.to);
        }
    }

    if (bestMoves.empty()) return {Position(-1, -1), Position(-1, -1)};

    size_t idx = prng(0, static_cast<uint32_t>(bestMoves.size() - 1));
    return {bestMoves[idx].first, bestMoves[idx].second};
}
