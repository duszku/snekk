#include "game.h"

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
