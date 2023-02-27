#include "logic.h"

static void      spawn_apple(struct game *, unsigned *);

void *
logic_entry_point(void *v_game)
{
        struct       game *game;
        unsigned     seed;
        int          over;

        game = (struct game *)v_game;
        seed = getpid();
        over = 0;

        while (!over) {
                spawn_apple(game, &seed);
                sleep(1);

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
spawn_apple(struct game *game, unsigned *seed_p)
{
        int      ap_x, ap_y;

        do {
                do ap_x = rand_r(seed_p);
                while (ap_x < 1 || (unsigned)ap_x > game->g_widt - 1);

                do ap_y = rand_r(seed_p);
                while (ap_y < 1 || (unsigned)ap_y > game->g_heig - 1);
        } while (0); /* TODO: check for snake collision */

        if (pthread_mutex_lock(&(game->mt_apple)) != 0)
                ERROR("pthread_mutex_lock");

        *((int *)ftuple_fst(game->apple)) = ap_x;
        *((int *)ftuple_snd(game->apple)) = ap_y;

        if (pthread_mutex_unlock(&(game->mt_apple)) != 0)
                ERROR("pthread_mutex_lock");
}
