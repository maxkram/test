CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99
LDFLAGS = -lncurses

all: game_of_life

game_of_life: src/game_of_life.c
	$(CC) $(CFLAGS) -o game_of_life src/game_of_life.c $(LDFLAGS)

clean:
	rm -f game_of_life
