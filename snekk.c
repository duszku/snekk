#include "game.h"
#include "ui.h"

void         tup_free(void *);          /* frees a coordinate tuple */
void         init(struct game *);       /* initializes game struct */
void         cleanup(struct game *);    /* cleans the game struct */

int
main(void)
{
        struct   game game;

        init(&game);

        return EXIT_SUCCESS;
}

void
tup_free(void *v_tup)
{
        struct   ftuple *tup;
        void    *tmp;

        tup = (struct ftuple *)v_tup;

        tmp = ftuple_fst(tup);
        free(tmp);
        tmp = ftuple_snd(tup);
        free(tmp);

        ftuple_free(&tup);
}

void
init(struct game *g)
{
#define NL(X) ((X) == NULL)

        struct   ftuple *sn_head;
        int     *pos_x;
        int     *pos_y;

        /* basic game data */
        g->g_heig   = DEFAULT_HEIGHT;
        g->g_widt   = DEFAULT_WIDTH;
        g->points   = 0;
        g->gameover = 0;

        /* setting up snake */
        if (NL(pos_x = malloc(sizeof(int))) || NL(pos_y = malloc(sizeof(int))))
                ERROR("malloc");

        *pos_x = g->g_widt >> 1;
        *pos_y = g->g_heig >> 1;

        if (NL(sn_head = ftuple_create(2, pos_x, pos_y)))
                ERROR("ftuple_create");

        if (NL(g->snake = flist_append(NULL, sn_head, FLIST_CLEANABLE)))
                ERROR("flist_append");

        flist_set_cleanup(g->snake, tup_free);

        /* preparing mutexes */
        if (pthread_mutex_init(&(g->mt_apple), NULL) != 0)
                ERROR("pthread_mutex_init");

        if (pthread_mutex_init(&(g->mt_snake), NULL) != 0)
                ERROR("pthread_mutex_init");
}

void
cleanup(struct game *g)
{
}
