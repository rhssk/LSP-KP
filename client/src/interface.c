#include <ncurses.h>
#include <unistd.h>
#include "debug_macros.h"
#include "common.h"
#include "interface.h"

#define GRID_Y_BLOCKS           8
#define GRID_X_BLOCKS           GRID_Y_BLOCKS
#define GRID_BLOCK_Y_SIZE       4
#define GRID_BLOCK_X_SIZE       GRID_BLOCK_Y_SIZE * 2
#define GRID_Y                  GRID_Y_BLOCKS * GRID_BLOCK_Y_SIZE
#define GRID_X                  GRID_X_BLOCKS * GRID_BLOCK_X_SIZE

void init_screen(void)
{
    WINDOW *grid;
    int max_x, max_y, ch;

    /* Set up the main window */
    check(initscr() != NULL, "Error when initializing the main window");
    check(start_color() == 0, "Error when initializing colors");
    init_pair(1, COLOR_WHITE, COLOR_BLACK);
    init_pair(2, COLOR_WHITE, COLOR_GREEN);
    init_pair(3, COLOR_RED, COLOR_RED);
    bkgd(COLOR_PAIR(1));
    cbreak(); /* Disable buffering of typed characters */
    noecho(); /* Don't echo typed characters */
    keypad(stdscr, TRUE); /* Enable use of special keys */
    touchwin(stdscr); /* Refresh all windows */

    getmaxyx(stdscr, max_y, max_x); /* Get window dimensions */
    if (max_y < GRID_Y || max_x < GRID_X)
        log_err("Avaiable screen size is too small, at least %d x %d is needed",
                GRID_X, GRID_Y);

    grid = newwin(GRID_Y, GRID_X, 0, 0);
    draw_grid(grid);

    getchar();
    endwin();
error:
    ;
}

void draw_grid(WINDOW *grid)
{
    int x, y;

    box(grid, 0, 0);
    for (y = GRID_BLOCK_Y_SIZE; y < GRID_Y - 2; y += GRID_BLOCK_Y_SIZE) {
        mvwhline(grid, y, 1, ACS_HLINE, GRID_X - 2);
        for (x = GRID_BLOCK_X_SIZE; x < GRID_X - 2; x += GRID_BLOCK_X_SIZE) {
            mvwvline(grid, 1, x, ACS_VLINE, GRID_Y - 2);
        }
    }
    wrefresh(grid);
}
