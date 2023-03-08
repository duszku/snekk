/*
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF ACCOMPANYING ECLIPSE
 * PUBLIC LICENSE ("LICENSE"). ANY USE, REPRODUCTION OR DISTRIBUTION
 * OF THE PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT.
 */

#ifndef GAME_H
#define GAME_H

#define _POSIX_C_SOURCE 200809L

#define DEFAULT_WIDTH   80
#define DEFAULT_HEIGHT  30

#include <errno.h>
#include <locale.h>
#include <pthread.h>
#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <unistd.h>

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

/* Similar in purpose to WinAPI's UNREFERENCED_PARAMETER */
#define UNUSED(X) ((void)(X))

struct game {
        /* player movement direction */
        enum { STOP = 0, U, D, L, R } dir;

        struct       flist *snake;  /* list of coords of snake segments */
        struct       ftuple *apple; /* coordinates of current apple position */
        unsigned     g_widt;        /* width of the map */
        unsigned     g_heig;        /* height of the map */
        unsigned     points;        /* # points player has */
        unsigned     rng_s;         /* rng seed */
        unsigned     gameover : 1;  /* has player lost? */

        pthread_mutex_t mt_snake;
        pthread_mutex_t mt_apple;
        pthread_mutex_t mt_gover;
        pthread_mutex_t mt_dir;
};

void         set_handler(void (*)(int), int);       /* sets signal handler */
void         sigint_block(sigset_t *, sigset_t *);  /* block a sigset */

#endif /* GAME_H */
