CC=gcc
TARGET=DEBUG

SRC=
OBJ=${SRC:.c=.o}
DIR=bin

# Compiler flags
C_FLAGS_RELES=-Wall -O2
C_FLAGS_DEBUG=-Wall -Wextra -ansi -pedantic -ggdb -Og -DTARGET_DEBUG
C_FLAGS_SANIT=-Wall -Wextra -ggdb -Og -fsanitize=address,undefined

# Libraries
LIBS_BASE=-lpthread -lfuncc $(pkg-config --libs ncurses)

LIBS_RELES=${LIBS_BASE}
LIBS_DEBUG=${LIBS_BASE}
LIBS_SANIT=-lasan -lubsan ${LIBS_BASE}

# Targets
.PHONY: all snekk clean

all: snekk clean

snekk: ${OBJ} ${DIR}
	${CC} -o ${DIR}/$@ ${OBJ} ${LIBS_${TARGET}}

.c.o:
	${CC} ${C_FLAGS_${TARGET}} -c -o $@ $<

clean:
	rm -f *.o

${DIR}:
	mkdir ${DIR}
