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
