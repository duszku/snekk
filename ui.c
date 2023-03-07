/*
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF ACCOMPANYING ECLIPSE
 * PUBLIC LICENSE ("LICENSE"). ANY USE, REPRODUCTION OR DISTRIBUTION
 * OF THE PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT.
 */

#include "ui.h"

#define INSERT_CHAR(X, Y, C) do {   \
        move((Y), (X));             \
        delch();                    \
        insch((C));                 \
} while (0)

/*
 * terminal offsets so that map can be printed in the center, needs to be global
 * because flist_map() takes a function that only takes one argument as an input
 */
int      x_off = 0;
int      y_off = 0;

static volatile sig_atomic_t last_sig = 0;
static void remember_sig(int sig) { last_sig = sig; }

static WINDOW       *curses_setup(void);         /* abstracts boilerplate */
static void          draw_empty(struct game *);  /* draws empty map & borders */
static void          draw_map(struct game *);    /* draws snake & apple */
static void          pop_input(struct game *);   /* reads input queue and sets
                                                    parameters in the game
                                                    struct accordingly */
static void          calc_offsets(WINDOW *);     /* calculates global offsets */

static void         *map_helper(void *);

void *
ui_entry_point(void *v_game)
{
        struct       game *game;
        WINDOW      *wnd;
        sigset_t     mask, oldmask;
        int          over;

        sigint_block(&mask, &oldmask);
        set_handler(remember_sig, SIGUSR1);

        game = (struct game *)v_game;
        over = 0;

        wnd = curses_setup();
        calc_offsets(wnd);
        while (!over) {
                pop_input(game);
                draw_empty(game);
                draw_map(game);

                sigsuspend(&oldmask);

                if (pthread_mutex_lock(&(game->mt_gover)) != 0)
                        ERROR("pthread_mutex_lock");

                if (game->gameover)
                        over = 1;

                if (pthread_mutex_unlock(&(game->mt_gover)) != 0)
                        ERROR("pthread_mutex_unlock");
        }

        endwin();

        pthread_sigmask(SIG_UNBLOCK, &mask, NULL);

        return NULL;
}

WINDOW *
curses_setup(void)
{
        WINDOW *ret;

        ret = initscr();
        cbreak();
        noecho();
        nodelay(stdscr, 1);
        keypad(stdscr, 1);
        clear();
        refresh();

        return ret;
}

void
draw_empty(struct game *g)
{
        unsigned x, y;

        clear();
        refresh();

        /* draw vertical lines */
        for (y = y_off; y < g->g_heig + y_off; ++y) {
                INSERT_CHAR(x_off, y, '|');
                INSERT_CHAR(g->g_widt - 1 + x_off, y, '|');
        }

        /* draw horizontal lines */
        for (x = x_off; x < g->g_widt + x_off; ++x) {
                INSERT_CHAR(x, y_off, '-');
                INSERT_CHAR(x, g->g_heig - 1 + y_off, '-');
        }

        /* draw corners */
        INSERT_CHAR(x_off, y_off, '+');
        INSERT_CHAR(x_off, g->g_heig - 1 + y_off, '+');
        INSERT_CHAR(g->g_widt - 1 + x_off, y_off, '+');
        INSERT_CHAR(g->g_widt - 1 + x_off, g->g_heig - 1 + y_off, '+');

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
                INSERT_CHAR(x + x_off, y + y_off, '@');

        /* drawing snake */
        if (pthread_mutex_lock(&(g->mt_snake)) != 0)
                ERROR("pthread_mutex_lock");

        flist_map(g->snake, map_helper, 0);

        x = *((int *)ftuple_fst(flist_val_head(g->snake)));
        y = *((int *)ftuple_snd(flist_val_head(g->snake)));

        if (pthread_mutex_unlock(&(g->mt_snake)) != 0)
                ERROR("pthread_mutex_unlock");

        if (x > 0 && y > 0)
                INSERT_CHAR(x + x_off, y + y_off, 'O');

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

        INSERT_CHAR(x + x_off, y + y_off, 'o');

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
calc_offsets(WINDOW *wnd)
{
        getmaxyx(wnd, y_off, x_off);

        y_off = (y_off - DEFAULT_HEIGHT) >> 1;
        x_off = (x_off - DEFAULT_WIDTH) >> 1;
}
