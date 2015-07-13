CC=gcc
LIBS=-lSDL2main -lSDL2 -lSDL2_image

all:
	$(CC) main.c -o blockPainter $(LIBS)
