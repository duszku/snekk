/*
 * THIS PROGRAM IS PROVIDED UNDER THE TERMS OF ACCOMPANYING ECLIPSE
 * PUBLIC LICENSE ("LICENSE"). ANY USE, REPRODUCTION OR DISTRIBUTION
 * OF THE PROGRAM CONSTITUTES RECIPIENT'S ACCEPTANCE OF THIS AGREEMENT.
 */

#ifndef UI_H
#define UI_H

#include "game.h"
#include <curses.h>

/*
 * This file serves one purpose and one purpose only and it is to expose this
 * particular subroutine to the main thread. Actual implementation is hidden
 * within `ui.c` to increase modularity of the code and make any future
 * refactorings simpler.
 *
 * As name suggests, following subroutine is expected to be used as the UI
 * thread's entry point.
 */
void        *ui_entry_point(void *);

#endif /* UI_H */
