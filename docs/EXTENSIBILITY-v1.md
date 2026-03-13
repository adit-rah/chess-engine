# Extensibility & Design Principles

**Version:** 1  
**Last Updated:** March 2025

---

## Versioning

Documents in `docs/` follow the pattern `NAME-vN.md` where N is an integer (1, 2, 3, …). When making substantive changes, create a new file with the next version number. Previous versions are kept for history. See [docs/INDEX.md](INDEX.md) for the current document index.

---

## 1. Layered Extensibility

The chess engine is built so that **most extensions require changes only at higher layers**. Lower levels are stable and should rarely be modified.

### 1.1 The Layering Model

```
┌─────────────────────────────────────────────────────────────────┐
│  Highest — main.cc, CLI, Web bindings                            │
│  • Add flags, commands, new entry points                         │
└─────────────────────────────────────────────────────────────────┘
                                    │
┌─────────────────────────────────────────────────────────────────┐
│  GameController                                                   │
│  • Wire new player types, displays, game modes                   │
└─────────────────────────────────────────────────────────────────┘
                                    │
┌─────────────────────────────────────────────────────────────────┐
│  Players, AI, Displays (Strategy/Observer implementations)      │
│  • Add AILevel5, new display backends, new player types          │
└─────────────────────────────────────────────────────────────────┘
                                    │
┌─────────────────────────────────────────────────────────────────┐
│  Board, Piece hierarchy                                          │
│  • Add new piece types, variants (e.g. Chess960)                 │
└─────────────────────────────────────────────────────────────────┘
                                    │
┌─────────────────────────────────────────────────────────────────┐
│  Lowest — Subject/Observer, Position, Utils (PRNG, Window)       │
│  • Stable; avoid changes unless necessary                        │
└─────────────────────────────────────────────────────────────────┘
```

---

## 2. Extending Without Touching Lower Layers

### 2.1 New AI Level

Add a new AI level (e.g. `AILevel6`) by:

1. Creating `ailevel6.h` and `ailevel6.cc` extending `ComputerPlayer`
2. Implementing `determineNextBestMove(Board &b)`
3. Registering in `GameController::createPlayerFromString()` with a new string (e.g. `"computer6"`)

**No changes required in:** Board, Piece, Subject, Observer, core logic.

### 2.2 New Display

Add a new display (e.g. `TerminalDisplay`, `WebSocketDisplay`) by:

1. Creating a class extending `AbstractDisplay`
2. Implementing `notify(Subject &whoFrom)` and casting to `Board&`
3. Instantiating and calling `game.attachDisplay(...)` in `main.cc` (or equivalent entry point)

**No changes required in:** Board, GameController internals, Piece, patterns.

### 2.3 New Player Type

Add a new player (e.g. `NetworkPlayer`) by:

1. Extending `Player` and implementing `makeMove(Board &board, std::istringstream &iss)`
2. Adding creation logic in `createPlayerFromString()` or a factory

**No changes required in:** Board, Piece, displays.

### 2.4 New Piece Type (e.g. variant chess)

Add a new piece by:

1. Creating a class extending `Piece`
2. Implementing `getRawMoves(const Board &b)` (and optionally `getValidMoves` for special rules)
3. Adding the type to `PieceType` enum and the Board’s copy/setup logic

**Minimal changes in:** `piece.h` (enum), `board.cc` (switch in copy/placePiece). No changes to GameController, players, displays, or patterns.

---

## 3. When to Modify Lower Layers

**Lower layers (Board internals, Piece base class, Subject/Observer, Position, utils) should not be changed unless:**

1. **Performance** — Profiling shows a bottleneck in core logic (e.g. move generation, board copies, observer iteration).
2. **Explicit feature request** — A requirement directly affects that layer (e.g. new game state, new rule, new board size).
3. **Bug fix** — A defect is located in that layer.

**Rationale:**

- Lower layers are reused by many higher-level components.
- Changes here have wide impact and increase regression risk.
- Most features can be implemented by adding new implementations of existing interfaces (Strategy, Observer) or new branches in factory methods.

---

## 4. Design Patterns Enabling Extensibility

| Pattern        | Role in extensibility                         |
|----------------|------------------------------------------------|
| **Strategy**   | New AI levels and player types without Board changes |
| **Observer**   | New displays without Board or GameController changes |
| **Template Method** | New pieces override `getRawMoves`/`getValidMoves`; base logic stays in Board |
| **Factory**    | `createPlayerFromString()` centralizes player creation; new types = new branches |

---

## 5. Summary

- **Prefer adding over changing** — New AI levels, displays, and player types extend existing abstractions.
- **Lower layers are stable** — Modify them only for performance, explicit requirements, or bug fixes.
- **Version documents when they change** — Use `NAME-vN.md` and increment N for substantive updates.
