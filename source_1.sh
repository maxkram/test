#!/bin/bash

# Create project structure
mkdir -p game_of_life_project/src
mkdir -p game_of_life_project/patterns

# Create game_of_life.c
cat > game_of_life_project/src/game_of_life.c << 'EOF'
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <ncurses.h>

#define WIDTH 80
#define HEIGHT 25
#define DELAY_STEP 50000
#define DELAY_MIN 50000
#define DELAY_MAX 1000000
#define DELAY_INIT 200000

typedef struct {
    int grid[HEIGHT][WIDTH];
} Field;

void load_field(Field *field);
void update_field(Field *current, Field *next);
int count_neighbors(Field *field, int y, int x);
void draw_field(Field *field);
void copy_field(Field *src, Field *dest);
void game_loop(Field *field);

int main(void) {
    Field current = {0};
    load_field(&current);
    initscr();
    noecho();
    curs_set(FALSE);
    timeout(0);
    keypad(stdscr, TRUE);
    game_loop(&current);
    endwin();
    return 0;
}

void load_field(Field *field) {
    char ch;
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            if (scanf(" %c", &ch) == 1) {
                field->grid[y][x] = (ch == '1') ? 1 : 0;
            } else {
                field->grid[y][x] = 0;
            }
        }
    }
}

void game_loop(Field *field) {
    int delay = DELAY_INIT;
    Field next = {0};

    while (1) {
        clear();
        draw_field(field);
        refresh();
        update_field(field, &next);
        copy_field(&next, field);
        usleep(delay);

        int ch = getch();
        if (ch == 'A' || ch == 'a') delay = (delay > DELAY_MIN) ? delay - DELAY_STEP : delay;
        if (ch == 'Z' || ch == 'z') delay = (delay < DELAY_MAX) ? delay + DELAY_STEP : delay;
        if (ch == ' ') break;
    }
}

void draw_field(Field *field) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            mvaddch(y, x, field->grid[y][x] ? 'O' : ' ');
        }
    }
}

void update_field(Field *current, Field *next) {
    for (int y = 0; y < HEIGHT; y++) {
        for (int x = 0; x < WIDTH; x++) {
            int n = count_neighbors(current, y, x);
            if (current->grid[y][x])
                next->grid[y][x] = (n == 2 || n == 3) ? 1 : 0;
            else
                next->grid[y][x] = (n == 3) ? 1 : 0;
        }
    }
}

int count_neighbors(Field *field, int y, int x) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0) continue;
            int ny = (y + dy + HEIGHT) % HEIGHT;
            int nx = (x + dx + WIDTH) % WIDTH;
            count += field->grid[ny][nx];
        }
    }
    return count;
}

void copy_field(Field *src, Field *dest) {
    for (int y = 0; y < HEIGHT; y++)
        for (int x = 0; x < WIDTH; x++)
            dest->grid[y][x] = src->grid[y][x];
}
EOF

# Create Makefile
cat > game_of_life_project/Makefile << 'EOF'
CC = gcc
CFLAGS = -Wall -Wextra -Werror -std=c99
LDFLAGS = -lncurses

all: game_of_life

game_of_life: src/game_of_life.c
	$(CC) $(CFLAGS) -o game_of_life src/game_of_life.c $(LDFLAGS)

clean:
	rm -f game_of_life
EOF

# Pattern helper function
create_empty_pattern() {
  yes "00000000000000000000000000000000000000000000000000000000000000000000000000000000" | head -25 > "$1"
}

# Create stable.txt (block)
create_empty_pattern game_of_life_project/patterns/stable.txt
sed -i '5s/00000000000000000000000000000000000000000000000000000000000000000000000000000000/00000000000000000000000000000000000000110000000000000000000000000000000000000000/' game_of_life_project/patterns/stable.txt
sed -i '6s/00000000000000000000000000000000000000000000000000000000000000000000000000000000/00000000000000000000000000000000000000110000000000000000000000000000000000000000/' game_of_life_project/patterns/stable.txt

# Create blinker.txt
create_empty_pattern game_of_life_project/patterns/blinker.txt
sed -i '5s/00000000000000000000000000000000000000000000000000000000000000000000000000000000/00000000000000000000000000000000000000111000000000000000000000000000000000000000/' game_of_life_project/patterns/blinker.txt

# Create glider.txt
create_empty_pattern game_of_life_project/patterns/glider.txt
sed -i '3s/00000000000000000000000000000000000000000000000000000000000000000000000000000000/00000000000000000000000000000000000000010000000000000000000000000000000000000000/' game_of_life_project/patterns/glider.txt
sed -i '4s/00000000000000000000000000000000000000000000000000000000000000000000000000000000/00000000000000000000000000000000000000101000000000000000000000000000000000000000/' game_of_life_project/patterns/glider.txt
sed -i '5s/00000000000000000000000000000000000000000000000000000000000000000000000000000000/00000000000000000000000000000000000001111000000000000000000000000000000000000000/' game_of_life_project/patterns/glider.txt

# Create toad.txt
create_empty_pattern game_of_life_project/patterns/toad.txt
sed -i '5s/00000000000000000000000000000000000000000000000000000000000000000000000000000000/00000000000000000000000000000000000000111000000000000000000000000000000000000000/' game_of_life_project/patterns/toad.txt
sed -i '6s/00000000000000000000000000000000000000000000000000000000000000000000000000000000/00000000000000000000000000000000000001110000000000000000000000000000000000000000/' game_of_life_project/patterns/toad.txt

# Create pulsar.txt (simplified partial pulsar)
create_empty_pattern game_of_life_project/patterns/pulsar.txt
sed -i '10s/.*/00000000000000000000111000000000000000000000000000000000000000000000000000000000/' game_of_life_project/patterns/pulsar.txt
sed -i '11s/.*/000000000000*
