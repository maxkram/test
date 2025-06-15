#define _POSIX_C_SOURCE 199309L

#include <ncurses.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

#define SIZE_X 80
#define SIZE_Y 25
#define DELAY_STEP 50
#define DELAY_MIN 50
#define DELAY_MAX 1000
#define DELAY_INIT 200

void load_field(int (*field)[SIZE_X + 2]);
void print_field(int (*field)[SIZE_X + 2]);
int new_generation(int (*current)[SIZE_X + 2], int (*next)[SIZE_X + 2]);
int count_neighbors(int (*field)[SIZE_X + 2], int y, int x);
void copy_margin(int (*field)[SIZE_X + 2]);
void game_loop(int (*field)[SIZE_X + 2]);

int main(void) {
    int field[SIZE_Y + 2][SIZE_X + 2] = {0};

    load_field(field);

    initscr();
    noecho();
    curs_set(0);
    keypad(stdscr, TRUE);
    timeout(1);
    game_loop(field);
    endwin();

    return 0;
}

void load_field(int (*field)[SIZE_X + 2]) {
    char ch;
    for (int y = 1; y <= SIZE_Y; y++) {
        for (int x = 1; x <= SIZE_X; x++) {
            if (fscanf(stdin, "%c", &ch) == 1) {
                field[y][x] = (ch == '1' || ch == '*') ? 1 : 0;
            } else {
                field[y][x] = 0;
            }
        }
    }
}

void print_field(int (*field)[SIZE_X + 2]) {
    clear();
    for (int y = 1; y <= SIZE_Y; y++) {
        for (int x = 1; x <= SIZE_X; x++) {
            mvaddch(y - 1, x - 1, field[y][x] ? '*' : ' ');
        }
    }
    refresh();
}

int new_generation(int (*current)[SIZE_X + 2], int (*next)[SIZE_X + 2]) {
    int changed = 0;
    for (int y = 1; y <= SIZE_Y; y++) {
        for (int x = 1; x <= SIZE_X; x++) {
            int neighbors = count_neighbors(current, y, x);
            int alive = current[y][x];
            next[y][x] = (alive && (neighbors == 2 || neighbors == 3)) || (!alive && neighbors == 3);
            if (next[y][x] != alive)
                changed = 1;
        }
    }
    return changed;
}

int count_neighbors(int (*field)[SIZE_X + 2], int y, int x) {
    int count = 0;
    for (int dy = -1; dy <= 1; dy++) {
        for (int dx = -1; dx <= 1; dx++) {
            if (dy == 0 && dx == 0)
                continue;
            int ny = (y + dy <= 0) ? SIZE_Y : (y + dy > SIZE_Y ? 1 : y + dy);
            int nx = (x + dx <= 0) ? SIZE_X : (x + dx > SIZE_X ? 1 : x + dx);
            count += field[ny][nx];
        }
    }
    return count;
}

void copy_margin(int (*field)[SIZE_X + 2]) {
    for (int y = 0; y <= SIZE_Y + 1; y++) {
        field[y][0] = field[y][SIZE_X];
        field[y][SIZE_X + 1] = field[y][1];
    }
    for (int x = 0; x <= SIZE_X + 1; x++) {
        field[0][x] = field[SIZE_Y][x];
        field[SIZE_Y + 1][x] = field[1][x];
    }
}

void game_loop(int (*field)[SIZE_X + 2]) {
    int next[SIZE_Y + 2][SIZE_X + 2] = {0};
    int delay = DELAY_INIT;
    int running = 1;

    copy_margin(field);

    while (running) {
        print_field(field);
        copy_margin(field);
        if (!new_generation(field, next))
            break;

        for (int y = 0; y <= SIZE_Y + 1; y++)
            for (int x = 0; x <= SIZE_X + 1; x++)
                field[y][x] = next[y][x];

        int key = getch();
        if ((key == 'A' || key == 'a') && delay > DELAY_MIN)
            delay -= DELAY_STEP;
        else if ((key == 'Z' || key == 'z') && delay < DELAY_MAX)
            delay += DELAY_STEP;
        else if (key == ' ')
            running = 0;

        napms(delay);
    }
}
