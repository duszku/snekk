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
