CXX = g++-14
CXXFLAGS = -std=c++20 -Wall -MMD -Werror=vla

EXEC = chess
GRAPHICS_EXEC = chess_graphics

# Base objects (text-only version)
BASE_OBJECTS = \
    main.o \
    gamecontroller.o \
    board.o \
    subject.o \
    textdisplay.o \
    piece.o rook.o bishop.o queen.o knight.o pawn.o king.o emptypiece.o \
    scoredposition.o \
    humanplayer.o computerplayer.o ailevel1.o ailevel2.o ailevel3.o ailevel4.o

# Graphics-specific objects
GRAPHICS_OBJECTS = graphicsdisplay.o window.o

# Dependencies
DEPENDS = ${BASE_OBJECTS:.o=.d} ${GRAPHICS_OBJECTS:.o=.d}

# Default build: text-only chess
${EXEC}: ${BASE_OBJECTS}
	${CXX} ${CXXFLAGS} ${BASE_OBJECTS} -o ${EXEC}

# Graphics build: adds graphicsdisplay + window + X11
${GRAPHICS_EXEC}: ${BASE_OBJECTS} ${GRAPHICS_OBJECTS}
	${CXX} ${CXXFLAGS} ${BASE_OBJECTS} ${GRAPHICS_OBJECTS} -o ${GRAPHICS_EXEC} -lX11

# Compile rule for any .cc
%.o: %.cc
	${CXX} ${CXXFLAGS} -c $< -o $@

# Auto-include dependencies
-include ${DEPENDS}

.PHONY: clean run graphics

# Run text-only
run: ${EXEC}
	./${EXEC}

# Build + run graphics version
graphics: ${GRAPHICS_EXEC}
	./${GRAPHICS_EXEC}

# Clean everything
clean:
	rm -f ${BASE_OBJECTS} ${GRAPHICS_OBJECTS} ${EXEC} ${GRAPHICS_EXEC} ${DEPENDS}
