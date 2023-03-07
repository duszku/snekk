/*
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF ACCOMPANYING ECLIPSE
 * PUBLIC LICENSE ("LICENSE"). ANY USE, REPRODUCTION OR DISTRIBUTION
 * OF THE PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT.
 */

#include "game.h"
#include "logic.h"
#include "ui.h"

#define TEMPO_IN_MS 300

void         tup_free(void *);                  /* frees a coordinate tuple */
void         init(struct game *);               /* initializes game struct */
void         cleanup(struct game *);            /* cleans the game struct */
void         wait_till_over(struct game *);     /* waits for SIGINT */

/* SIGINT handling */
volatile sig_atomic_t end = 0;
void sigint_handl(int ign) { UNUSED(ign); end = 1; }

int
main(void)
{
        struct       game game;
        pthread_t    ui_tid, lg_tid;

        setlocale(LC_ALL, "");
        set_handler(sigint_handl, SIGINT);
        init(&game);

        if (pthread_create(&ui_tid, NULL, ui_entry_point, &game) != 0)
                ERROR("pthread_create");

        if (pthread_create(&lg_tid, NULL, logic_entry_point, &game) != 0)
                ERROR("pthread_create");

        while (!end) {
                nap_ms(TEMPO_IN_MS);
                pthread_kill(lg_tid, SIGUSR1);
                pthread_kill(ui_tid, SIGUSR1);

                if (pthread_mutex_lock(&(game.mt_gover)) != 0)
                        ERROR("pthread_mutex_lock");
                if (game.gameover)
                        end = 1;
                if (pthread_mutex_unlock(&(game.mt_gover)) != 0)
                        ERROR("pthread_mutex_unlock");
        }

        if (pthread_mutex_lock(&(game.mt_gover)) != 0)
                ERROR("pthread_mutex_lock");
        game.gameover = 1;
        if (pthread_mutex_unlock(&(game.mt_gover)) != 0)
                ERROR("pthread_mutex_unlock");

        pthread_join(ui_tid, NULL);
        pthread_join(lg_tid, NULL);
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

        srand(time(NULL));

        /* basic game data */
        g->g_heig   = DEFAULT_HEIGHT;
        g->g_widt   = DEFAULT_WIDTH;
        g->rng_s    = rand();
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
        if (pthread_mutex_init(&(g->mt_gover), NULL) != 0)
                ERROR("pthread_mutex_init");
        if (pthread_mutex_init(&(g->mt_dir), NULL) != 0)
                ERROR("pthread_mutex_init");
}

void
cleanup(struct game *g)
{
        flist_free(&(g->snake), 0);
        tup_free(g->apple);

        if (pthread_mutex_destroy(&(g->mt_apple)) != 0)
                ERROR("pthread_mutex_destroy");
        if (pthread_mutex_destroy(&(g->mt_snake)) != 0)
                ERROR("pthread_mutex_destroy");
        if (pthread_mutex_destroy(&(g->mt_gover)) != 0)
                ERROR("pthread_mutex_destroy");
        if (pthread_mutex_destroy(&(g->mt_dir)) != 0)
                ERROR("pthread_mutex_destroy");
}
