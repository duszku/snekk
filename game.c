#include "game.h"

void
nap_ms(unsigned ns)
{
#define MSEC_IN_NSEC(X) ((X) * 1000000)

        struct   timespec nap;
        int      ret;

        nap.tv_nsec = MSEC_IN_NSEC(ns);
        nap.tv_sec  = 0;

        do ret = nanosleep(&nap, &nap);
        while (ret != 0 && errno == EINTR);

        if (ret != 0)
                ERROR("nanosleep");
}

void
set_handler(void (*f)(int), int sig)
{
        struct   sigaction sa;

        memset(&sa, 0x00, sizeof(struct sigaction));
        sa.sa_handler = f;

        if (sigaction(sig, &sa, NULL) == -1)
                ERROR("sigaction");
}

void
sigint_block(sigset_t *p_mask, sigset_t *p_oldmask)
{
        sigemptyset(p_mask);
        sigaddset(p_mask, SIGINT);
        pthread_sigmask(SIG_BLOCK, p_mask, p_oldmask);
}
