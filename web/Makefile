# Emscripten build for chess engine (WASM)
# Requires: emcc (from Emscripten SDK)
#
# If emcc is not in PATH, use one of:
#   make EMSDK=~/emsdk
#   make EMCC=/path/to/emcc
#
# To install: git clone https://github.com/emscripten-core/emsdk.git && cd emsdk && ./emsdk install latest && ./emsdk activate latest

EMCC_PATH := $(if $(EMCC),$(EMCC),$(if $(EMSDK),$(EMSDK)/upstream/emscripten/emcc,emcc))
CXX = $(EMCC_PATH)
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

# When EMSDK is set, run build in bash with emsdk env (ensures BINARYEN_ROOT etc. are set)
ifdef EMSDK
  BUILD = bash -c '. $(EMSDK)/emsdk_env.sh >/dev/null 2>&1 && $(CXX) $(CXXFLAGS) $(SOURCES) -o $(OUTPUT) $(LDFLAGS)'
else
  BUILD = $(CXX) $(CXXFLAGS) $(SOURCES) -o $(OUTPUT) $(LDFLAGS)
endif

all: $(OUTPUT)

$(OUTPUT): $(SOURCES)
	$(BUILD)

clean:
	rm -f $(OUTPUT) chess.wasm chess.wasm.map

help:
	@echo "Emscripten (emcc) required. Install with:"
	@echo "  git clone https://github.com/emscripten-core/emsdk.git ~/emsdk"
	@echo "  cd ~/emsdk && ./emsdk install latest && ./emsdk activate latest"
	@echo ""
	@echo "Then either:"
	@echo "  source ~/emsdk/emsdk_env.sh   # in each new terminal"
	@echo "  make"
	@echo ""
	@echo "Or run directly (no source needed):"
	@echo "  make EMSDK=$$HOME/dev/tools/emsdk"

.PHONY: all clean help
