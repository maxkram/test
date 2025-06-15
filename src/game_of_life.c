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
