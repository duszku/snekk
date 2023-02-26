#include "ui.h"

static void         draw_empty(struct game *);
static void         draw_map(struct game *);

void *
ui_thread_r(void *v_game)
{
        struct   game *game;

        game = (struct game *)v_game;

        initscr();
        cbreak();
        noecho();
        clear();
        refresh();
        draw_empty(game);

        while (!game->gameover) {
                draw_map(game);
                sleep(1);
        }

        endwin();

        return NULL;
}

void
draw_empty(struct game *g)
{
#define INSERT_CHAR(X, Y, C) do {   \
        move((Y), (X));             \
        delch();                    \
        insch((C));                 \
} while (0)

        unsigned x, y;

        for (y = 0; y < g->g_heig; ++y) {
                INSERT_CHAR(0, y, '|');
                INSERT_CHAR(g->g_widt - 1, y, '|');
        }

        for (x = 0; x < g->g_widt; ++x) {
                INSERT_CHAR(x, 0, '-');
                INSERT_CHAR(x, g->g_heig - 1, '-');
        }

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
