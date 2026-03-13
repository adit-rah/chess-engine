# Chess

A C++20 chess game with text-based display and modern graphical mode using SDL2. <br>
Supports human vs human, human vs AI, and AI vs AI with four difficulty levels.

> **Note:** This project has been migrated from X11 to SDL2 for better cross-platform support and modern graphics capabilities.

## Project Structure

```
chess-engine/
├── src/
│   ├── core/       # Game logic, board, and main entry point
│   ├── pieces/     # Chess piece implementations
│   ├── players/    # Human and computer player classes
│   ├── ai/         # AI difficulty levels (1-4)
│   ├── display/    # Text, graphics, and log displays
│   ├── patterns/   # Observer/Subject design patterns
│   └── utils/      # Utility classes (PRNG, window)
├── Makefile
└── README.md
```

## Requirements

- C++20 compiler (tested with g++)
- Make
- SDL2 and SDL2_ttf libraries (for graphics)

### Installing SDL2

**Ubuntu/Debian:**
```bash
sudo apt-get install libsdl2-dev libsdl2-ttf-dev
```

**Fedora/RHEL:**
```bash
sudo dnf install SDL2-devel SDL2_ttf-devel
```

**macOS (Homebrew):**
```bash
brew install sdl2 sdl2_ttf
```

**Windows (MSYS2):**
```bash
pacman -S mingw-w64-x86_64-SDL2 mingw-w64-x86_64-SDL2_ttf
```

## Building

**Important:** Make sure SDL2 libraries are installed first (see [Installing SDL2](#installing-sdl2) above).

Build the game:
```bash
make
```
This produces: `./chess`

Clean build artifacts:
```bash
make clean
```

## Running

Text-only mode:
```bash
./chess
```
With graphics:
```bash
./chess -g
```
With logging:
```bash
./chess -l
```
With both:
```bash
./chess -g -l
```

## Commands

- **Start a game**  
  ```
  game white-player black-player
  ```  
  Example:  
  ```
  game human computer1
  ```  
  where `computer1`–`computer5` are AI difficulty levels.

- **Make a move**  
  ```
  move e2 e4
  ```  
  or just `move` for AI moves.

- **Resign**  
  ```
  resign
  ```

- **Setup mode**  
  ```
  setup
  ```  
  Then use `+ K e1`, `- e1`, etc.

- **Quit**  
  Press `Ctrl+D` or type `quit`.

## Displays

- **TextDisplay**: ASCII chessboard  
- **GraphicsDisplay**: Modern SDL2-based GUI with hardware acceleration
- **LogDisplay**: optional file-based move log  

Multiple displays can be attached simultaneously. The graphics display features:
- Cross-platform support (Windows, Linux, macOS)
- Smooth rendering with hardware acceleration
- Modern, clean interface

## AI Levels

- `computer1`: random moves  
- `computer2`: simple heuristics  
- `computer3`: improved heuristics  
- `computer4`: 2-ply lookahead  
- `computer5`: strongest AI (4-ply alpha-beta, static eval, piece-square tables, quiescence)  

## Web Build (WASM)

The engine can be compiled to WebAssembly for the browser. See [web/README.md](web/README.md) for build instructions. A minimal web UI is included; deploy the `web` folder to GitHub Pages or any static host.

## Example

```
$ ./chess
Welcome to Chess!
Commands:
  game white-player black-player
  move e2 e4
  autoplay
  resign
  setup
  Ctrl+D to quit.

> game human computer1
> move e2 e4
```
