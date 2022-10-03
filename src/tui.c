#include "tui.h"
#include "project.h"
#include <stdio.h>

#define SCREEN_WIDTH  tb_width()
#define SCREEN_HEIGHT tb_height()

int tui_state = 0;

// 100 total
// Number: 4 cell
// name: 40 percent (min 20 cell)
// score: 3 cell
// start_date: len(str(date))
// last_update: len(str(date))
// time_spent: len(str(date))
// width == 100
// x    == 4
// x = (width * 4) / 100

int cursor_pos = 0;

void draw_screen() {
  int x = 0, y = 1, rln, fmt_padding, padding_val = 4;
  int i = 0, fg = TB_DEFAULT, bg = TB_DEFAULT;

  if (project_length == 0)
    return;

  tb_clear();
  tb_printf(0, 0, TB_BLACK | TB_BOLD, TB_WHITE, 
      "% *s% *s",
      -4, "No",
      (-1 * tb_width()), " Name"
  );
  ++y;
  for (i = 0; i < project_length; ++i) {
    if (i == cursor_pos) {
      fg = TB_BLINK | TB_BLACK | TB_BOLD;
      bg = TB_WHITE;
    } else 
      fg = bg = TB_DEFAULT;

    rln = abs(i - cursor_pos);
    fmt_padding = (rln == 0) ? padding_val * -1 : padding_val;
    
    tb_printf(x, y++, fg, bg, "% *d % *s", fmt_padding,
        (rln == 0) ? cursor_pos + 1 : rln,
        
        -1 * (10 * SCREEN_WIDTH) / 55, indexable_projects[i]->name
        );
  }

  tb_printf(0, SCREEN_HEIGHT - 3, TB_BLACK, TB_WHITE, 
      "path: %-*s ln:%2d", SCREEN_WIDTH - 6 - 6, indexable_projects[cursor_pos]->directory, cursor_pos + 1);
  tb_printf(0, SCREEN_HEIGHT - 2, TB_DEFAULT, TB_DEFAULT, "quit: q");
  tb_printf(0, SCREEN_HEIGHT - 1, TB_DEFAULT, TB_DEFAULT, "down: j");
  tb_present();
}

/* void draw_screen() { */
/*   int x = 0, y = 1, rln, fmt_padding, padding_val = 4; */
/*   int i = 0, fg = TB_DEFAULT, bg = TB_DEFAULT; */

/*   if (project_length == 0) */
/*     return; */

/*   tb_clear(); */
/*   tb_printf(0, 0, TB_BLACK | TB_BOLD, TB_WHITE, */ 
/*       "%*s%*s%*s%*s%*s%*s", */ 
/*       -1 * 5, "No", */
/*       -1 * (10 * SCREEN_WIDTH) / 55, "Name", */
/*       -1 * (10 * SCREEN_WIDTH) / 55, "Score", */
/*       -1 * (10 * SCREEN_WIDTH) / 55, "Start", */
/*       -1 * (10 * SCREEN_WIDTH) / 55, "Last", */
/*       -1 * (10 * SCREEN_WIDTH) / 40, "Time Spent" */
/*   ); */
/*   ++y; */
/*   for (i = 0; i < project_length; ++i) { */
/*     if (i == cursor_pos) { */
/*       fg = TB_BLINK | TB_BLACK | TB_BOLD; */
/*       bg = TB_WHITE; */
/*     } else */ 
/*       fg = bg = TB_DEFAULT; */

/*     rln = abs(i - cursor_pos); */
/*     fmt_padding = (rln == 0) ? padding_val * -1 : padding_val; */
    
/*     tb_printf(x, y++, fg, bg, "% *d % *s%*f%*lld%*lld%*lld", fmt_padding, */
/*         (rln == 0) ? cursor_pos + 1 : rln, */
        
/*         -1 * (10 * SCREEN_WIDTH) / 55, indexable_projects[i]->name, */
/*         -1 * (10 * SCREEN_WIDTH) / 55, indexable_projects[i]->score, */
/*         -1 * (10 * SCREEN_WIDTH) / 55, indexable_projects[i]->start_date, */
/*         -1 * (10 * SCREEN_WIDTH) / 55, indexable_projects[i]->last_update, */
/*         -1 * (10 * SCREEN_WIDTH) / 40, indexable_projects[i]->time_spent */
/*         ); */
/*   } */

/*   tb_printf(0, SCREEN_HEIGHT - 3, TB_BLACK, TB_WHITE, */ 
/*       "path: %-*s ln:%2d", SCREEN_WIDTH - 6 - 6, indexable_projects[cursor_pos]->directory, cursor_pos + 1); */
/*   tb_printf(0, SCREEN_HEIGHT - 2, TB_DEFAULT, TB_DEFAULT, "quit: q"); */
/*   tb_printf(0, SCREEN_HEIGHT - 1, TB_DEFAULT, TB_DEFAULT, "down: j"); */
/*   tb_present(); */
/* } */

void handle_key(struct tb_event ev) {
  uint32_t key = ev.key + ev.ch;
  switch(key) {
    case TB_KEY_ARROW_DOWN:
    case 'j':
      ++cursor_pos;
      cursor_pos = (cursor_pos > project_length - 1)? 0 : cursor_pos;
      break;
    case TB_KEY_ARROW_UP:
    case 'k':
      --cursor_pos;
      cursor_pos = (cursor_pos < 0)? project_length - 1 : cursor_pos;
      break;
    case TB_KEY_ENTER:
    case 'l':
      tb_shutdown();
      tui_state = 0;
      activate_project(indexable_projects[cursor_pos]);
      break;
    case '/':
      break;
    case TB_KEY_ESC:
    case 'q':
      tui_state = 0;
      tb_shutdown();
      break;
  }
}
