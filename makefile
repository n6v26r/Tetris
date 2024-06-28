all: 
	gcc main.c display.h tetris.h utils.h -lncurses -o tetris

run: all
	./tetris
