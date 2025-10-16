# Chess

A C++20 chess game with text-based display and graphical mode using X11. <br>
Supports human vs human, human vs AI, and AI vs AI with four difficulty levels.

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

- C++20 compiler (tested with g++-14)
- Make
- X11 libraries (for graphics)

## Building

Default build (text-only):
```bash
make
```
This produces: `./chess`

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
  where `computer1`–`computer4` are AI difficulty levels.

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
- **GraphicsDisplay**: X11 GUI  
- **LogDisplay**: optional file-based move log  

Multiple displays can be attached simultaneously.

## AI Levels

- `computer1`: random moves  
- `computer2`: simple heuristics  
- `computer3`: improved heuristics  
- `computer4`: strongest AI  

## Cleaning

```bash
make clean
```

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
