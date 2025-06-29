#include "./title/title.h"
#include <ncurses.h>

void init();

int main(int argc, char **argv) {
  init();
  title();

  endwin();
  return 0;
}

void init() {
  initscr();
  crmode();
  noecho();
  curs_set(0);
  start_color();
  init_pair(1, COLOR_RED, COLOR_BLACK);
  init_pair(2, COLOR_RED, COLOR_RED);
  init_pair(3, COLOR_RED, COLOR_BLUE);
  init_pair(4, COLOR_RED, COLOR_GREEN);
  init_pair(5, COLOR_RED, COLOR_WHITE);
  // timeout(1);
  nodelay(stdscr, TRUE);
}