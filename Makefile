CXX = g++-14
CXXFLAGS = -std=c++20 -Wall -MMD -Werror=vla

EXEC = chess

# List all .o files
OBJECTS = \
    main.o \
    gamecontroller.o \
    board.o \
    subject.o \
    textdisplay.o \
    piece.o rook.o bishop.o queen.o knight.o pawn.o king.o emptypiece.o \
    scoredposition.o \
    humanplayer.o computerplayer.o ailevel1.o ailevel2.o ailevel3.o

DEPENDS = ${OBJECTS:.o=.d}

# Link step
${EXEC}: ${OBJECTS}
	${CXX} ${CXXFLAGS} ${OBJECTS} -o ${EXEC}

# Compile each .cc -> .o with auto dependency
-include ${DEPENDS}

%.o: %.cc
	${CXX} ${CXXFLAGS} -c $< -o $@

.PHONY: clean run

clean:
	rm -f ${OBJECTS} ${EXEC} ${DEPENDS}

run: ${EXEC}
	./${EXEC}
