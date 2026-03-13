# Chess Engine Architecture

**Version:** 1.0  
**Last Updated:** March 2025  
**C++ Standard:** C++20

---

## Table of Contents

1. [Overview](#1-overview)
2. [High-Level Architecture](#2-high-level-architecture)
3. [Core Module](#3-core-module)
4. [Pieces Module](#4-pieces-module)
5. [Players Module](#5-players-module)
6. [AI Module](#6-ai-module)
7. [Display Module](#7-display-module)
8. [Patterns Module (Observer/Subject)](#8-patterns-module-observer-subject)
9. [Utils Module](#9-utils-module)
10. [Data Flow & Execution](#10-data-flow--execution)
11. [Web/WASM Build](#11-webwasm-build)
12. [Design Nuances & Implementation Details](#12-design-nuances--implementation-details)

---

## 1. Overview

This is a C++20 chess engine supporting:

- **Game modes:** Human vs Human, Human vs AI, AI vs AI
- **Display modes:** Text (ASCII), SDL2 graphics, optional move logging
- **AI levels:** Four difficulty levels (random → heuristic → minimax-style)

The architecture follows classical OOP design patterns: **Strategy** (players), **Observer** (displays), and **Template Method** (piece movement validation).

**See also:** [EXTENSIBILITY-v1.md](EXTENSIBILITY-v1.md) — how to extend the system and when to modify lower layers.

---

## 2. High-Level Architecture

```
┌─────────────────────────────────────────────────────────────────────────┐
│                           main.cc (Entry Point)                           │
│  • Parses -g (graphics), -l (log) flags                                   │
│  • Creates GameController, attaches displays, runs command loop          │
└─────────────────────────────────────────────────────────────────────────┘
                                      │
                                      ▼
┌─────────────────────────────────────────────────────────────────────────┐
│                         GameController                                    │
│  • Owns Board, Player[2], displays[], turn, scores                       │
│  • Command parsing: game, move, resign, autoplay, setup                  │
│  • Delegates moves to current Player, notifies displays via Board        │
└─────────────────────────────────────────────────────────────────────────┘
           │                    │                     │
           ▼                    ▼                     ▼
    ┌──────────┐         ┌──────────────┐     ┌──────────────┐
    │  Board   │         │ Player[2]    │     │ displays[]   │
    │ (Subject)│         │ HumanPlayer  │     │ TextDisplay  │
    │          │         │ AILevel1-4   │     │ GraphicsDisp │
    └────┬─────┘         └──────────────┘     │ LogDisplay   │
         │                                     └──────────────┘
         │  Subject→Observer
         ▼
    Notify on state change
```

**Directory layout:**

```
src/
├── core/       # Board, GameController, main, ScoredPosition
├── pieces/     # Piece hierarchy (Piece, Pawn, Rook, Knight, Bishop, Queen, King, EmptyPiece)
├── players/    # Player, HumanPlayer, ComputerPlayer
├── ai/         # AILevel1–4
├── display/    # AbstractDisplay, TextDisplay, GraphicsDisplay, LogDisplay
├── patterns/   # Subject, Observer
└── utils/      # PRNG, SDLWindow (Xwindow alias)
```

---

## 3. Core Module

### 3.1 Board (`board.h`, `board.cc`)

**Inheritance:** `Board : public Subject`

The Board is the central game state. It owns a 2D array of `Piece*` (`pieces[row][col]`), implements chess rules, and acts as the Subject for the Observer pattern.

**Key responsibilities:**
- Piece placement and removal (for setup mode)
- Move validation and execution
- Special move handling: castling, en passant, promotion
- Game state queries: check, checkmate, stalemate, insufficient material

**Data model:**
- `Piece ***pieces` — 8×8 array of heap-allocated piece pointers
- `lastMoveFrom`, `lastMoveTo`, `lastMovePieceType` — for en passant and display
- `pendingPromotionChoice` — char `'Q'`/`'R'`/`'B'`/`'N'` for pawn promotion

**Validation flow:**
1. `movePiece(from, to)`:
   - Get valid moves from the piece via `piece->getValidMoves(*this)`
   - If `to` is in the list, try `handleSpecialMoves` first (castling, promotion, en passant)
   - Otherwise perform a normal move: swap piece, replace origin with `EmptyPiece`, delete captured piece

2. **Board-legality:** `isBoardLegalMove(from, to)`:
   - Temporarily applies the move, checks if the king is still attacked, then undoes the move

**Special moves (handled inside Board):**
- **Castling:** King moves 2 squares toward rook; rook jumps to the other side. Both must not have moved.
- **Promotion:** Pawn reaching last rank is replaced by `pendingPromotionChoice`.
- **En passant:** Pawn captures adjacent enemy pawn that just moved 2 squares.

### 3.2 GameController (`gamecontroller.h`, `gamecontroller.cc`)

**Responsibilities:**
- Owns `Board*`, `Player* players[2]`, `std::vector<AbstractDisplay*> displays`
- Parses text commands and delegates to handlers
- Creates players via `createPlayerFromString()` (e.g. `"human"`, `"computer2"`)
- On `startGame()`, attaches all displays to the Board as observers

**Command flow:**
- `processCommand(cmd)` → `handleSetupCommand` or `handleNormalCommand`
- Normal: `game`, `move`, `resign`, `autoplay`
- Setup: `+`, `-`, `=`, `done` (add/remove pieces, set turn)

**Move flow:**
1. `cmdMove(iss)` gets current player, calls `player->makeMove(board, iss)`
2. After move: `board->notifyObservers()`, then `checkGameState()`
3. If game over: update scores, `resetGame()`, `nextTurn()`

**Web API:**
- `tryHumanMove(from, to)` — validates and executes a human move
- `doComputerMove()` — executes computer move, returns move as string (e.g. `"e2e4"`)

### 3.3 ScoredPosition (`scoredposition.h`, `scoredposition.cc`)

A simple struct used by the AI:

- `Position from`, `Position to`
- `int score`

Constructors: `ScoredPosition(from, to, score)` and `ScoredPosition(from, r, c, s)`.

### 3.4 Position & Enums (`piece.h`)

- **Position:** `{ int row, int col }` — 0-indexed (row 0 = rank 1, col 0 = file a)
- **Colour:** `White`, `Black`, `None`
- **PieceType:** `None`, `Pawn`, `Bishop`, `Knight`, `Rook`, `Queen`, `King`

---

## 4. Pieces Module

### 4.1 Piece Hierarchy

```
Piece (abstract)
├── Pawn
├── Rook
├── Knight
├── Bishop
├── Queen
├── King
└── EmptyPiece
```

**Base `Piece` fields:**
- `value` (int) — used for captures and AI scoring (Pawn=1, Knight/Bishop=3, Rook=5, Queen=9, King=1e5)
- `type`, `colour`, `pos`, `hasMoved`

**Key methods:**
- `getRawMoves(const Board &b)` — **pure virtual** — squares the piece can reach (piece rules + blocking)
- `getValidMoves(Board &b)` — default implementation filters `getRawMoves` with `isBoardLegalMove`; overridden by Pawn and King for special moves

### 4.2 Movement Logic

- **Rook/Bishop/Queen:** Ray-casting until blocked; capture enemy, stop at own.
- **Knight:** Fixed 8 L-shaped offsets.
- **King:** Adjacent squares + castling (in `getValidMoves`). Castling checks: king/rook not moved, path empty, squares not attacked.
- **Pawn:** One/two forward, diagonal capture. En passant added in `getValidMoves` using `lastMoveFrom/To/Type`.

### 4.3 EmptyPiece

Represents empty squares. `getRawMoves()` returns an empty vector. Used to avoid `nullptr` in the board array.

---

## 5. Players Module

### 5.1 Player (abstract)

- `Colour colour`
- `virtual bool makeMove(Board &board, std::istringstream &iss) = 0`

### 5.2 HumanPlayer

- Parses `fromSq` and `toSq` from input (algebraic notation, e.g. `e2`).
- Optionally reads `promotionChoice` and sets `board.setPendingPromotion()`.
- Validates ownership and delegates to `board.movePiece(from, to)`.

### 5.3 ComputerPlayer

- Inherits `Player`, holds `PRNG prng`.
- `makeMove()` ignores input, calls `determineNextBestMove(board)`, then `board.movePiece(from, to)`.
- Abstract: `virtual std::vector<Position> determineNextBestMove(Board &b) = 0` — implemented by AI levels.

---

## 6. AI Module

All AI levels extend `ComputerPlayer` and implement `determineNextBestMove()`.

| Level | Strategy |
|-------|----------|
| **AILevel1** | Collect all legal moves, pick one randomly |
| **AILevel2** | Score moves: +piece value for capture, +1000 for checkmate, +5 for check. Pick max, tie-break randomly. Uses board copy for lookahead. |
| **AILevel3** | Level 2 + piece safety: `getLowestAttackerValue()` for danger squares. Adjusts score for hanging pieces and capture safety (defended/undefended targets). |
| **AILevel4** | Level 3 + `evaluateOpponentBestMove()`: 2-ply minimax (my move → opponent’s best reply). Subtracts opponent’s best score from our move score. |

| **AILevel5** | 4-ply alpha-beta minimax, static eval (material + mobility + king safety), piece-square tables, move ordering, quiescence search. |

**Scoring components (Level 2–4):**
- Capture: +target value
- Checkmate: +1000
- Check: +2 or +5
- Piece in danger: `piece.getValue() - lowestAttackerValue`
- Opponent’s best reply (Level 4): `move_points -= opponentBest`

`ScoredPosition` is used to store `(from, to, score)` for move selection.

---

## 7. Display Module

### 7.1 AbstractDisplay

- Extends `Observer`, defines `notify(Subject &whoFrom) = 0`
- Used only with `Board` as Subject (displays `dynamic_cast` to `Board&`)

### 7.2 TextDisplay

- Renders ASCII board: rows 8→1, files a–h
- White = uppercase, Black = lowercase
- Empty: space (light) or `_` (dark)

### 7.3 GraphicsDisplay

- Uses `Xwindow` (alias for `SDLWindow`) — SDL2 wrapper
- Draws colored squares, piece symbols via SDL_ttf
- Caches `lastBoard[8][8]`; redraws only changed squares

### 7.4 LogDisplay

- Writes to `chess_log.txt` in algebraic form
- Tracks `whiteToMove`, `moveCount` for numbering
- Uses `getLastMoveFrom/To/PieceType` (called on every `notify` — may log even when no move, depending on when `notify` is invoked)

---

## 8. Patterns Module (Observer/Subject)

### 8.1 Subject

- `std::vector<Observer*> observers`
- `attach(Observer *o)` — adds observer
- `detach(Observer *o)` — declared but **not implemented**
- `notifyObservers()` — calls `ob->notify(*this)` for each observer

### 8.2 Observer

- `virtual void notify(Subject &whoFrom) = 0`

### 8.3 Usage

1. `GameController` attaches `AbstractDisplay*` instances to the Board.
2. After each move (or setup change), `board->notifyObservers()` is called.
3. Each display receives `notify(board)` and updates its view.

Displays are owned by `main` (or unique_ptr); the Board holds raw pointers. No explicit detach on shutdown.

---

## 9. Utils Module

### 9.1 PRNG

- Seeded pseudo-random number generator (from CS246 style)
- `operator()(l, u)` returns value in `[l, u]`
- Used by AI for tie-breaking among equally scored moves

### 9.2 SDLWindow (Xwindow)

- SDL2 wrapper for window, renderer, and SDL_ttf font
- `fillRectangle`, `drawString`, `flush`, `clear`
- Colour enum: White, Black, Red, Green, Blue, LightBrown, DarkBrown, Grey
- `using Xwindow = SDLWindow` — backward compatibility with older X11-based naming

---

## 10. Data Flow & Execution

### 10.1 Startup

```
main() → GameController()
       → attach TextDisplay (always)
       → attach GraphicsDisplay (if -g)
       → attach LogDisplay (if -l)
       → command loop: getline → processCommand(cmd)
```

### 10.2 Game Start

```
game human computer2
  → cmdGame() → createPlayerFromString("human", White), createPlayerFromString("computer2", Black)
  → startGame(white, black)
  → board = new Board(); board->resetBoard()
  → for each display: board->attach(d); d->notify(*board)
```

### 10.3 Move Execution

```
move e2 e4  (human)
  → cmdMove() → players[0]->makeMove(board, iss)
  → HumanPlayer parses "e2" "e4" → board.movePiece(from, to)
  → GameController: board->notifyObservers(); checkGameState(); nextTurn()

move  (computer)
  → cmdMove() → ComputerPlayer::makeMove() → determineNextBestMove() → board.movePiece(from, to)
  → same notify + check + nextTurn
```

### 10.4 Setup Mode

```
setup
  → inSetupMode = true
  → + K e1  → board->placePiece('K', pos)
  → - e1    → board->removePiece(pos)
  → = white → forcedTurn = White
  → done    → validateSetup(); inSetupMode = false
```

---

## 11. Web/WASM Build

`web/` compiles the engine to WebAssembly via Emscripten.

- **game_api.cc:** Binds C++ to JS: `chess_init`, `chess_human_move`, `chess_computer_move`, `chess_get_board`, `chess_get_turn`, `chess_is_running`, `chess_reset`
- **boardToFen():** Converts internal board state to a FEN-like string for the web UI
- The web app uses a minimal UI; the core `GameController` and `Board` are shared with the native build

---

## 12. Design Nuances & Implementation Details

### Memory

- **Board:** Owns all `Piece*`; manually allocates/deallocates. Copy constructor performs deep copy of pieces.
- **GameController:** Owns `Board*` and `Player* players[2]`; deletes them in destructor and `resetGame()`.
- **Displays:** Owned by `main`; Board holds raw `Observer*` pointers. No `detach` on shutdown.

### Const Correctness

- `isBoardLegalMove` is non-const because it temporarily mutates the board (move + undo) for king-safety check.
- Board comments note pin detection that alters state, hence some methods are not const.

### Coordinate System

- `Position(row, col)` — row 0 = rank 1, col 0 = file a
- Algebraic `"e4"` → `Position(3, 4)` (row 3, col 4)

### LogDisplay Behavior

- LogDisplay’s `notify()` uses `getLastMoveFrom/To/PieceType`. If `notify` is called when no move occurred (e.g. setup), it may log stale or invalid data. Typically `notify` is only called after a move, so this is usually correct.

### AILevel4 `evaluateOpponentBestMove`

- Parameter `count` starts at 2 (2-ply lookahead).
- Recursive: evaluates opponent moves, then `-evaluateOpponentBestMove(tempBoard, myColour, count-1)` for alternating sides.
- Base case at `count == 0` returns 0 (static eval).

### SDL2 Migration

- README notes migration from X11 to SDL2.
- `window.h` exposes `using Xwindow = SDLWindow`; GraphicsDisplay and other code still use the `Xwindow` name.

### Incomplete Implementation

- `Subject::detach()` is declared but has no implementation in `subject.cc`.

---

*Document generated from codebase analysis. For build/run instructions, see the project README.*
