# Emscripten build for chess engine (WASM)
# Requires: emcc (from Emscripten SDK)

CXX = emcc
CXXFLAGS = -std=c++20 -Wall -O2 -I../src/core -I../src/pieces -I../src/players -I../src/ai -I../src/display -I../src/patterns -I../src/utils
LDFLAGS = -lembind -s WASM=1 -s SINGLE_FILE=1 -s MODULARIZE=1 -s EXPORT_NAME=createChessModule -s NO_EXIT_RUNTIME=1 -s ALLOW_MEMORY_GROWTH=1

# Source files - EXCLUDE SDL2-dependent and display modules
SOURCES = \
	../src/core/gamecontroller.cc \
	../src/core/board.cc \
	../src/core/scoredposition.cc \
	../src/patterns/subject.cc \
	../src/pieces/piece.cc \
	../src/pieces/rook.cc \
	../src/pieces/bishop.cc \
	../src/pieces/queen.cc \
	../src/pieces/knight.cc \
	../src/pieces/pawn.cc \
	../src/pieces/king.cc \
	../src/pieces/emptypiece.cc \
	../src/players/humanplayer.cc \
	../src/players/computerplayer.cc \
	../src/ai/ailevel1.cc \
	../src/ai/ailevel2.cc \
	../src/ai/ailevel3.cc \
	../src/ai/ailevel4.cc \
	../src/ai/ailevel5.cc \
	game_api.cc

OUTPUT = chess.js

all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(CXX) $(CXXFLAGS) $(SOURCES) -o $(OUTPUT) $(LDFLAGS)

clean:
	rm -f $(OUTPUT) chess.wasm chess.wasm.map

.PHONY: all clean
