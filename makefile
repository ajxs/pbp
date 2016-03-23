CC=gcc
SDL_CFLAGS := $(shell sdl2-config --cflags)
SDL_LDFLAGS := $(shell sdl2-config --libs) -lSDL2_image
SOURCES=main.c
OBJECTS=$(SOURCES:.c=.o)
EXECUTABLE=pbp

all: $(SOURCES) $(EXECUTABLE)

$(EXECUTABLE): $(OBJECTS)
	$(CC) $(OBJECTS) $(SDL_LDFLAGS) -o $@

.c .o:
	$(CC) $(SDL_CFLAGS) $< -o $@

clean:
	rm *.o
