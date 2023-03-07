/*
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF ACCOMPANYING ECLIPSE
 * PUBLIC LICENSE ("LICENSE"). ANY USE, REPRODUCTION OR DISTRIBUTION
 * OF THE PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT.
 */

#include "logic.h"

#define DEREF_INT_OF(X) (*((int *)(X)))

/*
 * previous snake segment, used for movement in pull_snake(). it is stored as
 * global state because flist_map() exposes list for mapping only with
 * single-argument functions.
 */
struct ftuple   *movement_prev;

static void      move_snake(struct game *);
static void      spawn_apple(struct game *);
static void      grow_snake(struct game *);
static int       check_collisions(struct game *);

/* helper subroutines */
static void      init_global(void);
static int       apple_collides(struct game *, int, int);
static int       tup_cmp(const void *, const void *);
static void     *pull_snake(void *);
static void     *mov_u(void *);
static void     *mov_d(void *);
static void     *mov_l(void *);
static void     *mov_r(void *);
static void      get_ap_coords(struct game *, int *, int *);

void *
logic_entry_point(void *v_game)
{
        struct       game *game;
        sigset_t     mask;
        int          over;

        init_global();
        sigint_block(&mask);
        set_handler(SIG_IGN, SIGUSR1);

        game = (struct game *)v_game;
        over = 0;

        if (pthread_mutex_lock(&(game->mt_dir)) != 0)
                ERROR("pthread_mutex_lock");
        game->dir = STOP;
        if (pthread_mutex_unlock(&(game->mt_dir)) != 0)
                ERROR("pthread_mutex_unlock");

        spawn_apple(game);
        while (!over) {
                nap_ms(400);
                move_snake(game);
                if (check_collisions(game))
                        break;

                if (pthread_mutex_lock(&(game->mt_gover)) != 0)
                        ERROR("pthread_mutex_lock");

                if (game->gameover)
                        over = 1;

                if (pthread_mutex_unlock(&(game->mt_gover)) != 0)
                        ERROR("pthread_mutex_unlock");
        }

        free(ftuple_fst(movement_prev));
        free(ftuple_snd(movement_prev));
        ftuple_free(&movement_prev);

        pthread_sigmask(SIG_UNBLOCK, &mask, NULL);

        return NULL;
}

void
spawn_apple(struct game *game)
{
        int      new_x, new_y;

        do {
                new_x = rand_r(&(game->rng_s)) % (game->g_widt - 2) + 1;
                new_y = rand_r(&(game->rng_s)) % (game->g_heig - 2) + 1;
        } while (apple_collides(game, new_x, new_y));

        if (pthread_mutex_lock(&(game->mt_apple)) != 0)
                ERROR("pthread_mutex_lock");

        DEREF_INT_OF(ftuple_fst(game->apple)) = new_x;
        DEREF_INT_OF(ftuple_snd(game->apple)) = new_y;

        if (pthread_mutex_unlock(&(game->mt_apple)) != 0)
                ERROR("pthread_mutex_lock");
}

int
apple_collides(struct game *game, int x, int y)
{
        struct   ftuple *tup;
        int      ret;

        if ((tup = ftuple_create(2, &x, &y)) == NULL)
                ERROR("ftuple_create");

        if (pthread_mutex_lock(&(game->mt_snake)) != 0)
                ERROR("pthread_mutex_lock");

        ret = flist_elem(game->snake, tup_cmp, tup);

        if (pthread_mutex_unlock(&(game->mt_snake)) != 0)
                ERROR("pthread_mutex_unlock");

        ftuple_free(&tup);

        return ret;
}

int
tup_cmp(const void *v_a, const void *v_b)
{
        struct   ftuple *a, *b;
        int      a_x, a_y, b_x, b_y;

        a = (struct ftuple *)v_a;
        b = (struct ftuple *)v_b;

        a_x = DEREF_INT_OF(ftuple_fst(a));
        a_y = DEREF_INT_OF(ftuple_snd(a));

        b_x = DEREF_INT_OF(ftuple_fst(b));
        b_y = DEREF_INT_OF(ftuple_snd(b));

        return a_x == b_x && a_y == b_y ? 0 : 1;
}

void
move_snake(struct game *game)
{
        void    *(*movs[])(void *) = { NULL, mov_u, mov_d, mov_l, mov_r };
        int      dir;

        if (pthread_mutex_lock(&(game->mt_dir)) != 0)
                ERROR("pthread_mutex_lock");
        dir = game->dir;
        if (pthread_mutex_unlock(&(game->mt_dir)) != 0)
                ERROR("pthread_mutex_unlock");

        if (dir == STOP)
                return;

        if (pthread_mutex_lock(&(game->mt_snake)) != 0)
                ERROR("pthread_mutex_lock");

        /* move all segments of the snake */
        DEREF_INT_OF(ftuple_fst(movement_prev)) = -1;
        DEREF_INT_OF(ftuple_snd(movement_prev)) = -1;
        flist_map(game->snake, pull_snake, 0);

        /* move head of the snake */
        movs[dir](flist_val_head(game->snake));

        if (pthread_mutex_unlock(&(game->mt_snake)) != 0)
                ERROR("pthread_mutex_unlock");
}

void
init_global(void)
{
        int *x, *y;

        if ((x = malloc(sizeof(int))) == NULL)
                ERROR("malloc");

        if ((y = malloc(sizeof(int))) == NULL)
                ERROR("malloc");

        if ((movement_prev = ftuple_create(2, x, y)) == NULL)
                ERROR("ftuple_create");

        DEREF_INT_OF(ftuple_fst(movement_prev)) = -1;
        DEREF_INT_OF(ftuple_snd(movement_prev)) = -1;
}

void *
pull_snake(void *v_tup)
{

        struct   ftuple *tup;
        int      tmp_x, tmp_y;

        tup   = (struct ftuple *)v_tup;
        tmp_x = DEREF_INT_OF(ftuple_fst(movement_prev));
        tmp_y = DEREF_INT_OF(ftuple_snd(movement_prev));

        /* save coords for next iteration */
        DEREF_INT_OF(ftuple_fst(movement_prev)) = DEREF_INT_OF(ftuple_fst(tup));
        DEREF_INT_OF(ftuple_snd(movement_prev)) = DEREF_INT_OF(ftuple_snd(tup));

        /* assign new coords */
        if (tmp_x != -1 && tmp_y != -1) {
                DEREF_INT_OF(ftuple_fst(tup)) = tmp_x;
                DEREF_INT_OF(ftuple_snd(tup)) = tmp_y;
        }

        return v_tup;
}

void
grow_snake(struct game *game)
{
        struct   ftuple *new;
        int     *x, *y;

        if ((x = malloc(sizeof(int))) == NULL)
                ERROR("malloc");

        if ((y = malloc(sizeof(int))) == NULL)
                ERROR("malloc");

        if (pthread_mutex_lock(&(game->mt_snake)) != 0)
                ERROR("pthread_mutex_lock");

        *x  = DEREF_INT_OF(ftuple_fst(flist_val_head(game->snake)));
        *y  = DEREF_INT_OF(ftuple_snd(flist_val_head(game->snake)));
        new = ftuple_create(2, x, y);

        game->snake = flist_prepend(game->snake, new, FLIST_CLEANABLE);

        if (pthread_mutex_unlock(&(game->mt_snake)) != 0)
                ERROR("pthread_mutex_unlock");
}

void *
mov_u(void *v_tup)
{
        struct   ftuple *tup;

        tup = (struct ftuple *)v_tup;
        DEREF_INT_OF(ftuple_snd(tup))--;

        return v_tup;
}

void *
mov_d(void *v_tup)
{
        struct   ftuple *tup;

        tup = (struct ftuple *)v_tup;
        DEREF_INT_OF(ftuple_snd(tup))++;

        return v_tup;
}

void *
mov_l(void *v_tup)
{
        struct   ftuple *tup;

        tup = (struct ftuple *)v_tup;
        DEREF_INT_OF(ftuple_fst(tup))--;

        return v_tup;
}

void *
mov_r(void *v_tup)
{
        struct   ftuple *tup;

        tup = (struct ftuple *)v_tup;
        DEREF_INT_OF(ftuple_fst(tup))++;

        return v_tup;
}

int
check_collisions(struct game *game)
{
        struct   flist *cpy;
        int      ap_x, ap_y, over;
        unsigned x, y;

        over = 0;

        if (pthread_mutex_lock(&(game->mt_snake)) != 0)
                ERROR("pthread_mutex_lock");

        cpy = flist_copy(game->snake, NULL);
        flist_tail(&cpy, 0);

        if (flist_elem(cpy, tup_cmp, flist_val_head(game->snake)))
                over = 1;

        x = DEREF_INT_OF(ftuple_fst(flist_val_head(game->snake)));
        y = DEREF_INT_OF(ftuple_snd(flist_val_head(game->snake)));

        if (pthread_mutex_unlock(&(game->mt_snake)) != 0)
                ERROR("pthread_mutex_unlock");

        if (x <= 0 || x >= game->g_widt - 1|| y <= 0 || y >= game->g_heig - 1)
                over = 1;

        if (over) {
                flist_free(&cpy, 0);

                if (pthread_mutex_lock(&(game->mt_gover)) != 0)
                        ERROR("pthread_mutex_lock");
                game->gameover = 1;
                if (pthread_mutex_unlock(&(game->mt_gover)) != 0)
                        ERROR("pthread_mutex_unlock");

                return 1;
        }

        get_ap_coords(game, &ap_x, &ap_y);
        if (apple_collides(game, ap_x, ap_y)) {
                game->points++;
                grow_snake(game);
                spawn_apple(game);
        }

        return 0;
}

void
get_ap_coords(struct game *game, int *xp, int *yp)
{
        if (pthread_mutex_lock(&(game->mt_apple)) != 0)
                ERROR("pthread_mutex_lock");

        *xp = DEREF_INT_OF(ftuple_fst(game->apple));
        *yp = DEREF_INT_OF(ftuple_snd(game->apple));

        if (pthread_mutex_unlock(&(game->mt_apple)) != 0)
                ERROR("pthread_mutex_unlock");
}
