#ifndef __DRAW_H__
#define __DRAW_H__

#include <termbox.h>

extern int tui_state;

void draw_screen();
void handle_key(struct tb_event ev);

#endif
