/*
 * THE ACCOMPANYING PROGRAM IS PROVIDED UNDER THE TERMS OF THIS ECLIPSE
 * PUBLIC LICENSE ("AGREEMENT"). ANY USE, REPRODUCTION OR DISTRIBUTION
 * OF THE PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT.
 */

#include "ui.h"

#define INSERT_CHAR(X, Y, C) do {   \
        move((Y), (X));             \
        delch();                    \
        insch((C));                 \
} while (0)

static void          curses_setup(void);         /* abstracts boilerplate */
static void          draw_empty(struct game *);  /* draws empty map & borders */
static void          draw_map(struct game *);    /* draws snake & apple */
static void          pop_input(struct game *);   /* reads input queue and sets
                                                    parameters in the game
                                                    struct accordingly */

static void         *map_helper(void *);

void *
ui_entry_point(void *v_game)
{
        struct       game *game;
        int          over;

        game = (struct game *)v_game;
        over = 0;

        curses_setup();
        while (!over) {
                pop_input(game);
                draw_empty(game);
                draw_map(game);
                nap_ms(500);

                if (pthread_mutex_lock(&(game->mt_gover)) != 0)
                        ERROR("pthread_mutex_lock");

                if (game->gameover)
                        over = 1;

                if (pthread_mutex_unlock(&(game->mt_gover)) != 0)
                        ERROR("pthread_mutex_unlock");
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
        nodelay(stdscr, 1);
        keypad(stdscr, 1);
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
        int x, y;

        /* drawing an apple */
        if (pthread_mutex_lock(&(g->mt_apple)) != 0)
                ERROR("pthread_mutex_lock");

        x = *((int *)ftuple_fst(g->apple));
        y = *((int *)ftuple_snd(g->apple));

        if (pthread_mutex_unlock(&(g->mt_apple)) != 0)
                ERROR("pthread_mutex_unlock");

        if (x > 0 && y > 0)
                INSERT_CHAR(x, y, '@');

        /* drawing snake */
        if (pthread_mutex_lock(&(g->mt_snake)) != 0)
                ERROR("pthread_mutex_lock");

        flist_map(g->snake, map_helper, 0);

        x = *((int *)ftuple_fst(flist_val_head(g->snake)));
        y = *((int *)ftuple_snd(flist_val_head(g->snake)));

        if (pthread_mutex_unlock(&(g->mt_snake)) != 0)
                ERROR("pthread_mutex_unlock");

        if (x > 0 && y > 0)
                INSERT_CHAR(x, y, 'O');

        refresh();
}

void *
map_helper(void *v_tup)
{
        struct   ftuple *tup;
        int      x, y;

        tup = (struct ftuple *)v_tup;

        x = *((int *)ftuple_fst(tup));
        y = *((int *)ftuple_snd(tup));

        INSERT_CHAR(x, y, 'o');

        return v_tup;
}

void
pop_input(struct game *g)
{
        if (pthread_mutex_lock(&(g->mt_dir)) != 0)
                ERROR("pthread_mutex_lock");

        switch (getch()) {
        case KEY_DOWN:
                /* FALLTHROUGH */
        case 's':
                g->dir = D;
                break;
        case KEY_UP:
                /* FALLTHROUGH */
        case 'w':
                g->dir = U;
                break;
        case KEY_LEFT:
                /* FALLTHROUGH */
        case 'a':
                g->dir = L;
                break;
        case KEY_RIGHT:
                /* FALLTHROUGH */
        case 'd':
                g->dir = R;
                break;
        case 'p':
                g->dir = STOP;
                break;
        case 'x':
                if (pthread_mutex_lock(&(g->mt_gover)) != 0)
                        ERROR("pthread_mutex_lock");

                g->gameover = 1;

                if (pthread_mutex_unlock(&(g->mt_gover)) != 0)
                        ERROR("pthread_mutex_unlock");
                break;
        default:
                break;
        }

        if (pthread_mutex_unlock(&(g->mt_dir)) != 0)
                ERROR("pthread_mutex_unlock");
}

void
nap_ms(unsigned ns)
{
#define MSEC_IN_NSEC(X) ((X) * 1000000)

        struct   timespec nap;
        int      ret;

        nap.tv_nsec = MSEC_IN_NSEC(ns);
        nap.tv_sec  = 0;

        do ret = nanosleep(&nap, &nap);
        while (ret != 0 && errno == EINTR);

        if (ret != 0)
                ERROR("nanosleep");
}
