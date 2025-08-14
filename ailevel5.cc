#include "ailevel5.h"
#include <algorithm>

Colour AILevel5::opposite(Colour c) {
    return (c == Colour::White) ? Colour::Black : Colour::White;
}

std::vector<Position> AILevel5::determineNextBestMove(Board &b) {
    int alpha = -INF, beta = INF;
    int bestScore = -INF;
    ScoredPosition bestMove(Position(-1,-1), Position(-1,-1), 0);

    auto moves = generateLegalMoves(b, me);
    if (moves.empty()) return {Position(-1,-1), Position(-1,-1)};

    orderMoves(b, me, moves);
    for (auto &m : moves) {
        Board child(b);
        child.movePiece(m.from, m.to);

        int score = -negativemax(child, opposite(me), maxDepth - 1, -beta, -alpha);
        if (score > bestScore) {
            bestScore = score;
            bestMove = m;
        }
        alpha = std::max(alpha, bestScore);
        if (alpha >= beta) break; // beta cut-off
    }

    return {bestMove.from, bestMove.to};
}

int AILevel5::eval(Board &b, Colour toMove) const {
    int score = 0;

    // Material
    for (int r = 0; r < b.getBoardSize(); ++r) {
        for (int c = 0; c < b.getBoardSize(); ++c) {
            Piece *p = b.getPieceAt(Position(r,c));
            if (!p) continue;
            int v = pieceValue(p->getType());
            score += (p->getColour() == me) ? v : -v;
        }
    }

    // Mobility
    int myMob  = (int)generateLegalMoves(const_cast<Board&>(b), me).size();
    int oppMob = (int)generateLegalMoves(const_cast<Board&>(b), opposite(me)).size();
    score += (myMob - oppMob);

    // Game state
    if (b.isCheckMate(opposite(me))) score += 100000;
    if (b.isCheckMate(me))          score -= 100000;
    if (b.isStaleMate(opposite(me))) score += 50;
    if (b.isStaleMate(me))           score -= 50;
    if (b.isInCheck(opposite(me))) score += 2;
    if (b.isInCheck(me))           score -= 2;

    return score;
}

int AILevel5::pieceValue(PieceType t) const {
    switch (t) {
        case PieceType::Pawn:   return 100;
        case PieceType::Knight: return 300;
        case PieceType::Bishop: return 320;
        case PieceType::Rook:   return 500;
        case PieceType::Queen:  return 900;
        case PieceType::King:   return 0;
        default: return 0;
    }
}

std::vector<ScoredPosition> AILevel5::generateLegalMoves(Board &b, Colour side) const {
    std::vector<ScoredPosition> out;
    const int N = b.getBoardSize();
    for (int r = 0; r < N; ++r) {
        for (int c = 0; c < N; ++c) {
            Position from(r,c);
            Piece *p = b.getPieceAt(from);
            if (!p || p->getColour() != side) continue;

            auto moves = p->getValidMoves(b);
            for (auto &to : moves) {
                if (!b.isBoardLegalMove(from, to)) continue;
                out.emplace_back(from, to, 0);
            }
        }
    }
    return out;
}

void AILevel5::orderMoves(Board &b, Colour side, std::vector<ScoredPosition> &moves) const {
    struct OrderData {
        int orderingScore;
        size_t index;
    };
    std::vector<OrderData> ordering;
    ordering.reserve(moves.size());

    for (size_t i = 0; i < moves.size(); ++i) {
        auto &m = moves[i];
        Piece *capt = b.getPieceAt(m.to);
        int capScore = 0;
        if (capt && capt->getColour() == opposite(side)) {
            capScore = 10 * pieceValue(capt->getType());
            Piece *att = b.getPieceAt(m.from);
            if (att) capScore -= pieceValue(att->getType());
        }

        int checkBonus = 0;
        {
            Board tmp(b);
            tmp.movePiece(m.from, m.to);
            if (tmp.isInCheck(opposite(side))) checkBonus = 5;
        }

        ordering.push_back({capScore + checkBonus, i});
    }

    std::stable_sort(ordering.begin(), ordering.end(),
        [](const OrderData &a, const OrderData &b){ return a.orderingScore > b.orderingScore; });

    std::vector<ScoredPosition> sorted;
    sorted.reserve(moves.size());
    for (auto &o : ordering) {
        sorted.push_back(moves[o.index]);
    }
    moves.swap(sorted);
}

int AILevel5::quiescence(Board &b, Colour toMove, int alpha, int beta) {
    int standPat = eval(b, toMove);
    if (standPat >= beta) return beta;
    if (alpha < standPat) alpha = standPat;

    auto moves = generateLegalMoves(b, toMove);
    moves.erase(std::remove_if(moves.begin(), moves.end(), [&](const ScoredPosition &m){
        Piece *capt = b.getPieceAt(m.to);
        return !(capt && capt->getColour() == opposite(toMove));
    }), moves.end());

    orderMoves(b, toMove, moves);

    for (auto &m : moves) {
        Board child(b);
        child.movePiece(m.from, m.to);
        int score = -quiescence(child, opposite(toMove), -beta, -alpha);
        if (score >= beta) return beta;
        if (score > alpha) alpha = score;
    }
    return alpha;
}

int AILevel5::negativemax(Board &b, Colour toMove, int depth, int alpha, int beta) {
    if (depth == 0) {
        return quiescence(b, toMove, alpha, beta);
    }

    auto moves = generateLegalMoves(b, toMove);
    if (moves.empty()) {
        if (b.isCheckMate(toMove)) return -100000 + (maxDepth - depth);
        return 0;
    }

    orderMoves(b, toMove, moves);

    int best = -INF;
    for (auto &m : moves) {
        Board child(b);
        child.movePiece(m.from, m.to);
        int score = -negativemax(child, opposite(toMove), depth - 1, -beta, -alpha);
        if (score > best) best = score;
        if (best > alpha) alpha = best;
        if (alpha >= beta) break;
    }
    return best;
}
