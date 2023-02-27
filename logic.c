#include "logic.h"

static void      spawn_apple(struct game *);

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

        do {
                ap_x = rand_r(&(game->rng_s)) % (game->g_widt - 1) + 1;
                ap_y = rand_r(&(game->rng_s)) % (game->g_heig - 1) + 1;
        } while (0); /* TODO: check for snake collision */

        if (pthread_mutex_lock(&(game->mt_apple)) != 0)
                ERROR("pthread_mutex_lock");

        *((int *)ftuple_fst(game->apple)) = ap_x;
        *((int *)ftuple_snd(game->apple)) = ap_y;

        if (pthread_mutex_unlock(&(game->mt_apple)) != 0)
                ERROR("pthread_mutex_lock");
}
