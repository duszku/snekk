#include "logic.h"

/*
 * previous snake segment, used for movement in pull_snake(). it is stored as
 * global state because flist_map() exposes list for mapping only with
 * single-argument functions.
 * */
struct ftuple   *movement_prev;

static void      move_snake(struct game *);
static void      spawn_apple(struct game *);

/* helper subroutines */
static int       apple_collides(struct game *, int, int);
static int       tup_cmp(const void *, const void *);
static void     *pull_snake(void *);
static void     *mov_u(void *);
static void     *mov_d(void *);
static void     *mov_l(void *);
static void     *mov_r(void *);

void *
logic_entry_point(void *v_game)
{
        struct       game *game;
        int          over;

        game = (struct game *)v_game;
        over = 0;

        while (!over) {
                sleep(1);
                spawn_apple(game);

                if (pthread_mutex_lock(&(game->mt_gover)) != 0)
                        ERROR("pthread_mutex_lock");

                if (game->gameover)
                        over = 1;

                if (pthread_mutex_unlock(&(game->mt_gover)) != 0)
                        ERROR("pthread_mutex_unlock");
        }

        return NULL;
}

void
spawn_apple(struct game *game)
{
        int      ap_x, ap_y;

        /*
         * TODO: (after movement is implemented) dont respawn apples if previous
         * one was not collected yet
         */
        do {
                ap_x = rand_r(&(game->rng_s)) % (game->g_widt - 1);
                ap_y = rand_r(&(game->rng_s)) % (game->g_heig - 1);
        } while (apple_collides(game, ap_x, ap_y));

        if (pthread_mutex_lock(&(game->mt_apple)) != 0)
                ERROR("pthread_mutex_lock");

        *((int *)ftuple_fst(game->apple)) = ap_x;
        *((int *)ftuple_snd(game->apple)) = ap_y;

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
#define DEREF_INT_OF(X) (*((int *)(X)))

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
