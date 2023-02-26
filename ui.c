#include "ui.h"

void *
ui_thread_r(void *v_game)
{
        printf("Hello from UI thread!\n");
        return NULL;
}
