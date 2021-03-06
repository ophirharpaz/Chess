#  -g    adds debugging information to the executable file
#  -Wall turns on most, but not all, compiler warnings
CC = gcc
CFLAGS  = -g -Wall

all: chessprog

clean: 
	$(RM) chessprog *.o *~

# To create the executable file count we need the object files
# countwords.o, counter.o, and scanner.o:
#
chessprog:  Flow.o Gui.o Minimax.o FileWriter.o DataStructures.o Chess.h
	$(CC) $(CFLAGS) -o chessprog Flow.o Gui.o Minimax.o FileWriter.o DataStructures.o -lm -std=c99 -pedantic-errors -g `sdl-config --libs` -L/usr/local/lib/SDL_image-1.2.12/lib -lSDL_image

Flow.o:  DataStructures.c DataStructures.h FileWriter.h FileWriter.c Minimax.h Minimax.c Gui.h Gui.c 
	$(CC) $(CFLAGS) -c Flow.c

Gui.o:  Flow.h Flow.c
	$(CC) $(CFLAGS) -c Gui.c

Minimax.o:  DataStructures.h  DataStructures.c Flow.h Flow.c 
	$(CC) $(CFLAGS) -c Minimax.c

FileWriter.o:  DataStructures.h DataStructures.c
	$(CC) $(CFLAGS) -c FileWriter.c

DataStructures.o:  Chess.h Minimax.h 
	$(CC) $(CFLAGS) -c DataStructures.c




