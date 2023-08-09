GC=gcc
FLAGS=-lSDL2 -lSDL2_image -lSDL2_ttf -fsanitize=address -lm

all: 
	$(GC) main.c sdl.c entities.c -o main $(FLAGS)