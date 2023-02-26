#ifndef GAME_H
#define GAME_H

#define _POSIX_C_SOURCE 200809L

#define DEFAULT_WIDTH   80
#define DEFAULT_HEIGHT  30

#include <locale.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include <funcc/flist.h>
#include <funcc/ftuple.h>

/* report an error and terminate entire process group */
#ifdef TARGET_DEBUG
# define ERROR(X) do {                                      \
        fprintf(stderr, "[%s:%d] ", __FILE__, __LINE__);    \
        perror((X));                                        \
        exit(EXIT_FAILURE);                                 \
  } while (0)
#else
# define ERROR(X) {;}
#endif

struct game {
        struct       flist *snake;  /* list of coords of snake segments */
        struct       ftuple *apple; /* coordinates of current apple position */
        unsigned     g_widt;        /* width of the map */
        unsigned     g_heig;        /* height of the map */
        unsigned     points;        /* # points player has */
        unsigned     gameover : 1;  /* has player lost? */

        pthread_mutex_t mt_snake;
        pthread_mutex_t mt_apple;
};

#endif /* GAME_H */
