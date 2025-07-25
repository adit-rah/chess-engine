#include "board.h"
#include "piece.h"
#include "emptypiece.h"
#include "pawn.h"
#include "rook.h"
#include "knight.h"
#include "bishop.h"
#include "queen.h"
#include "king.h"

#include <iostream> // this is no joke for tolower and toupper


// Constructor/Destructor 
Board::Board() {
    pieces = new Piece**[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; ++i) {
        pieces[i] = new Piece*[BOARD_SIZE];
        for (int j = 0; j < BOARD_SIZE; ++j) {
            pieces[i][j] = nullptr;
        }
    }
}


Board::Board(const Board& other) {
    // Allocate memory for pieces array (same as default constructor)
    pieces = new Piece**[BOARD_SIZE];
    for (int i = 0; i < BOARD_SIZE; ++i) {
        pieces[i] = new Piece*[BOARD_SIZE];
        for (int j = 0; j < BOARD_SIZE; ++j) {
            pieces[i][j] = nullptr;
        }
    }

    // Copy all pieces deeply
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            Piece* p = other.pieces[r][c];
            // Delete existing piece in destination if any
            if (pieces[r][c]) {
                delete pieces[r][c];
                pieces[r][c] = nullptr;
            }

            if (!p) {
                pieces[r][c] = nullptr;
                continue;
            }

            // Create a new piece of the same dynamic type
            switch (p->getType()) {
                case PieceType::Pawn:
                    pieces[r][c] = new Pawn(p->getColour(), Position(r, c));
                    break;
                case PieceType::Rook:
                    pieces[r][c] = new Rook(p->getColour(), Position(r, c));
                    break;
                case PieceType::Knight:
                    pieces[r][c] = new Knight(p->getColour(), Position(r, c));
                    break;
                case PieceType::Bishop:
                    pieces[r][c] = new Bishop(p->getColour(), Position(r, c));
                    break;
                case PieceType::Queen:
                    pieces[r][c] = new Queen(p->getColour(), Position(r, c));
                    break;
                case PieceType::King:
                    pieces[r][c] = new King(p->getColour(), Position(r, c));
                    break;
                case PieceType::None:
                    pieces[r][c] = new EmptyPiece(Position(r, c));
                    break;
            }
            // Copy hasMoved status if needed (add a getter if you don't have one)
            pieces[r][c]->setHasMoved(p->getHasMoved());
        }
    }

    // Copy any other Board member variables (like lastMoveFrom, pendingPromotionChoice, etc)
    lastMoveFrom = other.lastMoveFrom;
    lastMoveTo = other.lastMoveTo;
    lastMovePieceType = other.lastMovePieceType;
    pendingPromotionChoice = other.pendingPromotionChoice;
}


Board::~Board() {
    for (int i = 0; i < BOARD_SIZE; ++i) {
         for (int c = 0; c < BOARD_SIZE; ++c) {
            delete pieces[i][c];  // deletes the piece itself (can be nullptr safely)
        }
        delete[] pieces[i];
    }
    delete[] pieces; 
}


// Board Handling methods (see .h for documentation)

void Board::setPieces() {       // default chess board
    // === WHITE PIECES ===
    // Pawns
    for (int c = 0; c < BOARD_SIZE; ++c) {
        pieces[1][c] = new Pawn(Colour::White, Position(1, c));
    }
    // Back row
    pieces[0][0] = new Rook(Colour::White, Position(0,0));
    pieces[0][7] = new Rook(Colour::White, Position(0,7));
    pieces[0][1] = new Knight(Colour::White, Position(0,1));
    pieces[0][6] = new Knight(Colour::White, Position(0,6));
    pieces[0][2] = new Bishop(Colour::White, Position(0,2));
    pieces[0][5] = new Bishop(Colour::White, Position(0,5));
    pieces[0][3] = new Queen(Colour::White, Position(0,3));
    pieces[0][4] = new King(Colour::White, Position(0,4));

    // === BLACK PIECES ===
    // Pawns
    for (int c = 0; c < BOARD_SIZE; ++c) {
        pieces[6][c] = new Pawn(Colour::Black, Position(6, c));
    }
    // Back row
    pieces[7][0] = new Rook(Colour::Black, Position(7,0));
    pieces[7][7] = new Rook(Colour::Black, Position(7,7));
    pieces[7][1] = new Knight(Colour::Black, Position(7,1));
    pieces[7][6] = new Knight(Colour::Black, Position(7,6));
    pieces[7][2] = new Bishop(Colour::Black, Position(7,2));
    pieces[7][5] = new Bishop(Colour::Black, Position(7,5));
    pieces[7][3] = new Queen(Colour::Black, Position(7,3));
    pieces[7][4] = new King(Colour::Black, Position(7,4));

    // === EMPTY SQUARES ===
    for (int r = 2; r <= 5; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            pieces[r][c] = new EmptyPiece(Position(r,c)); // or new EmptyPiece if you prefer
        }
    }
}


void Board::emptyTheBoard() {
    for (int r = 0; r < BOARD_SIZE; ++r) {
        for (int c = 0; c < BOARD_SIZE; ++c) {
            // If there's already a piece, delete it first to avoid leaks
            if (pieces[r][c] != nullptr) {
                delete pieces[r][c];
            }
            // Replace with an EmptyPiece
            pieces[r][c] = new EmptyPiece(Position(r, c));
        }
    }

    // Reset last move tracking
    lastMoveFrom = Position(-1, -1);
    lastMoveTo = Position(-1, -1);
    lastMovePieceType = PieceType::None;
    pendingPromotionChoice = 'Q';

    // Notify displays so they update to show an empty board
    notifyObservers();
}


void Board::resetBoard() {
    setPieces();            // this should just work for now
}


// Move Validation methods (see .h for documentation)

Piece* Board::getPieceAt(Position p) const {
    if (p.row < 0 || p.row >= BOARD_SIZE || p.col < 0 || p.col >= BOARD_SIZE) {
        return nullptr; 
    }
    return pieces[p.row][p.col];
}


bool Board::isBoardLegalMove(Position from, Position to) { // this won't be const for speed
    Piece* moving = getPieceAt(from);
    if (!moving) return false;

    // Save original state
    Piece* captured = getPieceAt(to);

    // Temporarily move the piece
    pieces[to.row][to.col] = moving;
    moving->setPosition(to);
    pieces[from.row][from.col] = new EmptyPiece(from);

    // After move, find king
    Colour side = moving->getColour();
    Position kingPos = findKing(side);

    // Get all attacked squares by opponent
    Colour opp = (side == Colour::White ? Colour::Black : Colour::White);
    std::vector<Position> attackedSquares = squaresBeingAttackedBy(opp);

    // Check if king is in those attacked squares
    bool stillInCheck = false;
    for (Position &sq : attackedSquares) {
        if (sq == kingPos) {
            stillInCheck = true;
            break;
        }
    }

    // Undo the move
    delete pieces[from.row][from.col];  // remove temporary EmptyPiece
    pieces[from.row][from.col] = moving;
    moving->setPosition(from);
    pieces[to.row][to.col] = captured;

    return !stillInCheck;
}


bool Board::movePiece(Position from, Position to) {
    Piece* piece = getPieceAt(from);
    if (!piece) return false;           // piece just doesn't exist, invalid move

    std::vector<Position> validMoves = piece->getValidMoves(*this);

    // checks if the move is valid
    bool isValid = false;
    for (Position &m : validMoves) {
        if (m == to) { 
            isValid = true;
            break; 
        }
    }
    if (!isValid) return false;

    lastMoveFrom = from;
    lastMoveTo = to;
    lastMovePieceType = piece->getType();
    // then moves the piece if it does exist
    if (handleSpecialMoves(from, to)) {
        return true;
    }

    Piece *targetPiece = getPieceAt(to);
    // move the piece to the new position 
    pieces[to.row][to.col] = piece;
    piece->setPosition(to);
    piece->setHasMoved(true);

    // replace old position with empty piece
    pieces[from.row][from.col] = new EmptyPiece(from);

    // delete captured piece
    delete targetPiece;

    return true;
}


bool Board::handleSpecialMoves(Position from, Position to) {
    Piece* piece = getPieceAt(from);    // guard clause from earlier saves this
    if (!piece) return false;           // but just in case ;)

    if (piece->getType() == PieceType::King && handleCastling(from, to)) return true;
    if (piece->getType() == PieceType::Pawn && handlePromotion(from, to)) return true;
    if (piece->getType() == PieceType::Pawn && handleEnPassant(from, to)) return true;

    return false; 
}


bool Board::handleCastling(Position from, Position to) {
    int colDiff = to.col - from.col;
    if (std::abs(colDiff) != 2) return false; // must move 2 cols

    int row = from.row;
    bool kingSide = (colDiff > 0);

    Position rookFrom = kingSide ? Position(row, 7) : Position(row, 0);
    Position rookTo   = kingSide ? Position(row, 5) : Position(row, 3);

    Piece* rook = getPieceAt(rookFrom);
    if (!rook || rook->getType() != PieceType::Rook) return false;

    Piece* king = getPieceAt(from);

    // Move king
    pieces[to.row][to.col] = king;
    king->setPosition(to);
    king->setHasMoved(true);

    pieces[from.row][from.col] = new EmptyPiece(from);

    // Move rook
    pieces[rookTo.row][rookTo.col] = rook;
    rook->setPosition(rookTo);
    rook->setHasMoved(true);

    pieces[rookFrom.row][rookFrom.col] = new EmptyPiece(rookFrom);

    return true;
}


bool Board::handlePromotion(Position from, Position to) {
    Piece* pawn = getPieceAt(from);
    int lastRank = (pawn->getColour() == Colour::White ? 7 : 0);

    if (to.row != lastRank) return false;

    // normal move first
    Piece *targetPiece = getPieceAt(to);
    pieces[to.row][to.col] = pawn;
    pawn->setPosition(to);
    pawn->setHasMoved(true);

    pieces[from.row][from.col] = new EmptyPiece(from);
    delete targetPiece;

    // replace pawn with promoted piece
    char choice = pendingPromotionChoice; // set by HumanPlayer or the default ('Q')
    Piece* promotedPiece = nullptr;
    switch (choice) {
        case 'R': promotedPiece = new Rook(pawn->getColour(), to); break;
        case 'B': promotedPiece = new Bishop(pawn->getColour(), to); break;
        case 'N': promotedPiece = new Knight(pawn->getColour(), to); break;
        default:  promotedPiece = new Queen(pawn->getColour(), to); break;
    }

    delete pawn; // remove the old pawn
    pieces[to.row][to.col] = promotedPiece;

    return true;
}


bool Board::handleEnPassant(Position from, Position to) {
    // en passant requires diagonal move
    if (from.col == to.col) return false;

    Piece* target = getPieceAt(to);
    if (target && target->getType() != PieceType::None) return false; // must land on empty

    Piece* pawn = getPieceAt(from);
    int capturedRow = (pawn->getColour() == Colour::White) ? to.row - 1 : to.row + 1;
    Position capturedPawnPos(capturedRow, to.col);

    Piece* capturedPawn = getPieceAt(capturedPawnPos);
    if (!capturedPawn || capturedPawn->getType() != PieceType::Pawn) return false;

    // Perform en passant capture
    delete capturedPawn;
    pieces[capturedPawnPos.row][capturedPawnPos.col] = new EmptyPiece(capturedPawnPos);

    // Move pawn
    pieces[to.row][to.col] = pawn;
    pawn->setPosition(to);
    pawn->setHasMoved(true);

    pieces[from.row][from.col] = new EmptyPiece(from);

    return true;
}



bool Board::canMove(Piece& p) {
    std::vector<Position> validMoves = p.getValidMoves(*this);
    return !validMoves.empty();
}


// Placement Validation methods (see .h for documentation)

void Board::setPieceAt(Position pos, Piece* piece) {
    // First delete any existing piece at that square
    Piece* existing = pieces[pos.row][pos.col];
    if (existing) delete existing;

    // Place the new piece
    pieces[pos.row][pos.col] = piece;

    // Also update the piece’s internal position
    if (piece) {
        piece->setPosition(pos);
    }
}


void Board::placePiece(char pieceSymbol, Position pos) {
    // get piece info 
    Colour colour = isupper(pieceSymbol) ? Colour::White : Colour::Black;
    char lower = tolower(pieceSymbol);

    Piece* newPiece = nullptr;
    switch (lower) {
        case 'k': newPiece = new King(colour, pos); break;
        case 'q': newPiece = new Queen(colour, pos); break;
        case 'r': newPiece = new Rook(colour, pos); break;
        case 'b': newPiece = new Bishop(colour, pos); break;
        case 'n': newPiece = new Knight(colour, pos); break;
        case 'p': newPiece = new Pawn(colour, pos); break;
        default: newPiece = new EmptyPiece(pos); break; 
    }

    setPieceAt(pos, newPiece);  // place it
}


void Board::removePiece(Position pos) {
    setPieceAt(pos, new EmptyPiece(pos));
}


bool Board::validateSetup() const {
    int whiteKingCount = 0;
    int blackKingCount = 0;

    for (int r = 0; r < BOARD_SIZE; r++) {
        for (int c = 0; c < BOARD_SIZE; c++) {
            Piece* p = pieces[r][c];
            if (!p) continue;

            if (p->getType() == PieceType::King) {
                if (p->getColour() == Colour::White) whiteKingCount++;
                else if (p->getColour() == Colour::Black) blackKingCount++;
            }

            if (p->getType() == PieceType::Pawn && (r == 0 || r == 7)) {
                return false;
            }
        }
    }

    if (whiteKingCount != 1 || blackKingCount != 1) {
        return false;
    }

    return true;
}


// Attacking Logic and Handling methods (see .h for documentation)

std::vector<Position> Board::squaresBeingAttackedBy(Colour c) const {
    std::vector<Position> attackedSquares;
    for (int i = 0; i < BOARD_SIZE; ++i) {
        for (int j = 0; j < BOARD_SIZE; ++j) {
            Piece* piece = pieces[i][j];
            if (piece && piece->getColour() == c) {
                std::vector<Position> validMoves = piece->getRawMoves(*this); // squares being seen
                attackedSquares.insert(attackedSquares.end(), validMoves.begin(), validMoves.end());
            }
        }
    }
    return attackedSquares;
}


Position Board::findKing(Colour c) const {
    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            Piece* p = pieces[row][col];
            if (p != nullptr && p->getColour() == c && p->getType() == PieceType::King) {
                return Position(row, col);
            }
        }
    }
    return Position(-1, -1); // invalid position. also if it gets to this, smth broke 
}


bool Board::isInCheck(Colour c) const {
    Position kingPos = findKing(c);
    Colour opponent = (c == Colour::White) ? Colour::Black : Colour::White;

    std::vector<Position> opponentAttacks = squaresBeingAttackedBy(opponent);

    for (const Position& pos : opponentAttacks) {
        if (pos == kingPos) {
            return true;
        }
    }
    return false;
}



bool Board::isCheckMate(Colour c) {
    if (!isInCheck(c)) return false;

    for (int row = 0; row < BOARD_SIZE; ++row) {
        for (int col = 0; col < BOARD_SIZE; ++col) {
            Piece* p = pieces[row][col];
            if (!p || p->getColour() != c) continue;

            std::vector<Position> moves = p->getValidMoves(*this);

            if (!moves.empty()) return false; // found at least 1 legal move
        }
    }

    return true; // No valid moves left, it's checkmate
}


bool Board::isStaleMate(Colour c) {
    if (isInCheck(c)) return false;     // king in check means someone can still lose/win

    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
            Piece* p = pieces[row][col];
            if (!p || p->getColour() != c) continue;

            std::vector<Position> moves = p->getValidMoves(*this);

            if (!moves.empty()) return false; // found at least 1 legal move
        }
    }

    return true; // No valid moves left, it's stalemate
}


bool Board::insufficientMaterial() const {
    int minors = 0;
    for (int r = 0; r < 8; ++r) {
        for (int c = 0; c < 8; ++c) {
            Piece* p = getPieceAt({r,c});
            if (!p) continue;
            auto t = p->getType();

            // any heavy piece or pawn = enough material
            if (t == PieceType::Queen || t == PieceType::Rook || t == PieceType::Pawn) return false;

            // count bishops/knights
            if (t == PieceType::Bishop || t == PieceType::Knight) {
                minors++;
                if (minors > 1) return false; // 2 minors = still possible to mate
            }
        }
    }

    return true; // no mate possible
}


// Getters for the last move (see .h for documentation)

Position Board::getLastMoveFrom() const { return lastMoveFrom;  }

Position Board::getLastMoveTo() const {  return lastMoveTo;  }

PieceType Board::getLastMovePieceType() const {  return lastMovePieceType;  }

int Board::getBoardSize() const {   return BOARD_SIZE;  }

// Setter

void Board::setPendingPromotion(char p) {  pendingPromotionChoice = p;  }
