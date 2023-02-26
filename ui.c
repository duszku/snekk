#include "ui.h"

#define INSERT_CHAR(X, Y, C) do {   \
        move((Y), (X));             \
        delch();                    \
        insch((C));                 \
} while (0)

static void         curses_setup(void);         /* abstracts boilerplate code */
static void         draw_empty(struct game *);  /* draws empty map w/ borders */
static void         draw_map(struct game *);    /* draws snake & apple */

void *
ui_thread_r(void *v_game)
{
        struct   game *game;

        game = (struct game *)v_game;

        curses_setup();

        while (!game->gameover) {
                draw_empty(game);
                draw_map(game);
                sleep(1);
        }

        endwin();

        return NULL;
}

void
curses_setup(void)
{
        initscr();
        cbreak();
        noecho();
        clear();
        refresh();
}

void
draw_empty(struct game *g)
{
        unsigned x, y;

        clear();
        refresh();

        /* draw vertical lines */
        for (y = 0; y < g->g_heig; ++y) {
                INSERT_CHAR(0, y, '|');
                INSERT_CHAR(g->g_widt - 1, y, '|');
        }

        /* draw horizontal lines */
        for (x = 0; x < g->g_widt; ++x) {
                INSERT_CHAR(x, 0, '-');
                INSERT_CHAR(x, g->g_heig - 1, '-');
        }

        /* draw corners */
        INSERT_CHAR(0, 0, '+');
        INSERT_CHAR(0, g->g_heig - 1, '+');
        INSERT_CHAR(g->g_widt - 1, 0, '+');
        INSERT_CHAR(g->g_widt - 1, g->g_heig - 1, '+');

        refresh();
}

void
draw_map(struct game *g)
{
}
