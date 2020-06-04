.POSIX:
.DELETE_ON_ERROR:
MAKEFLAGS += --warn-undefined-variables
MAKEFLAGS += --no-builtin-rules

.PHONY: clean

CC       := gcc
CFLAGS   := -std=gnu11    \
	-O2                     \
	-g                      \
	-Wall                   \
	-Wextra                 \
	-Wmissing-prototypes    \
	-Wstrict-prototypes

SDL_CFLAGS  := ${shell sdl2-config --cflags}
SDL_LDLIBS  := ${shell sdl2-config --libs} -lSDL2_image
SOURCES     := main.c
OBJECTS     := ${SOURCES:.c=.o}

BINARY      := pbp

all: ${BINARY}

${BINARY}: ${OBJECTS}
	${CC} ${CFLAGS} ${OBJECTS} ${SDL_LDLIBS} -o $@

%.o: %.c
	${CC} -c $< -o $@ ${CFLAGS} ${SDL_CFLAGS}

clean:
	rm -f ${BINARY}
	rm -f *.o
