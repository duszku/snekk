#include "game.h"
#include "ui.h"

void         tup_free(void *);          /* frees a coordinate tuple */
void         init(struct game *);       /* initializes game struct */
void         cleanup(struct game *);    /* cleans the game struct */

int
main(void)
{
        struct       game game;
        pthread_t    ui_tid;

        init(&game);

        if (pthread_create(&ui_tid, NULL, ui_thread_r, &game) != 0)
                ERROR("pthread_create");

        printf("Hello from main thread!\n");
        pthread_join(ui_tid, NULL);
        cleanup(&game);

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
        int     *pos_x, *pos_y, *ap_x, *ap_y;

        /* basic game data */
        g->g_heig   = DEFAULT_HEIGHT;
        g->g_widt   = DEFAULT_WIDTH;
        g->points   = 0;
        g->gameover = 0;

        /* setting up snake */
        if (NL(pos_x = malloc(sizeof(int))))
                ERROR("malloc");
        if (NL(pos_y = malloc(sizeof(int))))
                ERROR("malloc");
        *pos_x = g->g_widt >> 1;
        *pos_y = g->g_heig >> 1;

        if (NL(sn_head = ftuple_create(2, pos_x, pos_y)))
                ERROR("ftuple_create");
        if (NL(g->snake = flist_append(NULL, sn_head, FLIST_CLEANABLE)))
                ERROR("flist_append");

        flist_set_cleanup(g->snake, tup_free);

        /* setting up for apple position */
        if (NL(ap_x = malloc(sizeof(int))))
                ERROR("malloc");
        if (NL(ap_y = malloc(sizeof(int))))
                ERROR("malloc");
        *ap_x = *ap_y = -1;

        if (NL(g->apple = ftuple_create(2, ap_x, ap_y)))
                ERROR("ftuple_create");

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
