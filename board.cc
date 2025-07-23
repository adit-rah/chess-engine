#include "board.h"
#include "piece.h"
#include "emptypiece.h"
#include "pawn.h"
#include "rook.h"
#include "knight.h"
#include "bishop.h"
#include "queen.h"
#include "king.h"


// Constructor/Destructor 
Board::Board() {
    pieces = new Piece**[8];
    for (int i = 0; i < 8; ++i) {
        pieces[i] = new Piece*[8];
        for (int j = 0; j < 8; ++j) {
            pieces[i][j] = nullptr;
        }
    }
}

Board::~Board() {
    for (int i = 0; i < 8; ++i) {
        delete pieces[i];
    }
    delete pieces; 
}


// Board Handling methods (see .h for documentation)

void Board::setPieces() {       // default chess board
    // === WHITE PIECES ===
    // Pawns
    for (int c = 0; c < 8; ++c) {
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
    for (int c = 0; c < 8; ++c) {
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
        for (int c = 0; c < 8; ++c) {
            pieces[r][c] = new EmptyPiece(Position(r,c)); // or new EmptyPiece if you prefer
        }
    }
}


void Board::resetBoard() {
    setPieces();            // this should just work for now
}


// Move Validation methods (see .h for documentation)

Piece* Board::getPieceAt(Position p) const {
    if (p.row < 0 || p.row >= 8 || p.col < 0 || p.col >= 8) {
        return nullptr; 
    }
    return pieces[p.row][p.col];
}


bool Board::isBoardLegalMove(Position from, Position to) { // this won't be const for speed
    Piece* movingPiece = getPieceAt(from);
    if (!movingPiece) return false;

    // Temporarily apply move (ie. remove the piece)
    pieces[from.row][from.col] = nullptr;

    // Check if own king is in check after move
    bool isValid = !isInCheck(movingPiece->getColour());

    // Revert move
    pieces[from.row][from.col] = movingPiece;

    return isValid;
}


bool Board::movePiece(Position from, Position to) {
    Piece* piece = getPieceAt(from);
    if (!piece) return false;           // piece just doesn't exist, invalid move

    std::vector<Position> validMoves = piece->getValidMoves(*this);

    // checks if the move is valid
    bool isValid = false;
    for (auto &m : validMoves) {
        if (m == to) { 
            isValid = true;
            break; 
        }
    }
    if (!isValid) return false;

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

    if (piece->getType() == PieceType::King) {
        // castling requires moving 2 columns
        int colDiff = to.col - from.col;
        if (std::abs(colDiff) != 2) return false;

        int row = from.row;
        
        // determine rook side
        bool kingSide = (colDiff > 0);

        Position rookFrom = kingSide ? Position(row, 7) : Position(row, 0);
        Position rookTo = kingSide ? Position(row, 5) : Position(row, 3);

        Piece* rook = getPieceAt(rookFrom);
        if (!rook || rook->getType() != PieceType::Rook) return false;

        pieces[to.row][to.col] = piece;
        piece->setPosition(to);
        piece->setHasMoved(true);

        pieces[from.row][from.col] = new EmptyPiece(from);

        // move rook
        pieces[rookTo.row][rookTo.col] = rook;
        rook->setPosition(rookTo);
        rook->setHasMoved(true);

        pieces[rookFrom.row][rookFrom.col] = new EmptyPiece(rookFrom);

        return true;
    }
    return false; 
}


bool Board::canMove(Piece& p) {
    std::vector<Position> validMoves = p.getValidMoves(*this);
    return !validMoves.empty();
}


// Placement Validation methods (see .h for documentation)

void Board::placePiece(char pieceSymbol, Position pos) {
    // Delete whatever is currently there
    if (pieces[pos.row][pos.col]) {
        delete pieces[pos.row][pos.col];
    }

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

    pieces[pos.row][pos.col] = newPiece;
}


void Board::removePiece(Position pos) {
    if (pieces[pos.row][pos.col]) {
        delete pieces[pos.row][pos.col];
    }
    pieces[pos.row][pos.col] = new EmptyPiece(pos);
}


bool Board::validateSetup() const {
    int whiteKingCount = 0;
    int blackKingCount = 0;

    for (int r = 0; r < 8; r++) {
        for (int c = 0; c < 8; c++) {
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

    // Cannot start with either king in check
    if (isInCheck(Colour::White) || isInCheck(Colour::Black)) {
        return false;
    }

    return true;
}


// Attacking Logic and Handling methods (see .h for documentation)

std::vector<Position> Board::squaresBeingAttackedBy(Colour c) const {
    std::vector<Position> attackedSquares;
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
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
    for (int row = 0; row < 8; ++row) {
        for (int col = 0; col < 8; ++col) {
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
    // Check if the player has any valid moves left
    for (int i = 0; i < 8; ++i) {
        for (int j = 0; j < 8; ++j) {
            Piece* piece = pieces[i][j];
            if (piece && piece->getColour() == c) {
                if (canMove(*piece)) {
                    return false; // Found a valid move, not checkmate
                }
            }
        }
    }
    return true; // No valid moves left, it's checkmate
}
