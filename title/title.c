#include "./title.h"
#include "../display/display.h"
#include "../game/game.h"
#include <ncurses.h>
#include <stdio.h>
#include <unistd.h>
void draw_title();
void help();
int display[H][W];
void title(char *file, char *cmd) {
  while (true) {
    draw_title();
    switch (getch()) {
      {
      case 'p':
        game(file, cmd);
        break;
      case 'h':
        help();
        break;
      case 'q':
        return;
      }
      usleep(50000);
    }
  }
};
void draw_title() {
  clear_display(display, 5);
  draw_text(display, 50, 20, "SWOLDIER", 3, 2);
  draw_text(display, 60, 80, "P_KEY - PLAY", 2, 2);
  draw_text(display, 60, 100, "H_KEY - HELP", 2, 2);
  draw_text(display, 60, 120, "Q_KEY - EXIT", 2, 2);
  print_display(display);
}
void help() {
  clear_display(display, 5);
  draw_text(display, 10, 5, "HELP", 1, 2);
  draw_text(display, 45, 30, "W S A D - MOVE", 2, 2);
  draw_text(display, 45, 60, "SPACE - ATTACK", 2, 2);
  draw_text(display, 45, 90, "K KEY - DUDGE", 2, 2);
  draw_text(display, 250, 130, "P_KEY - BACK", 1, 2);
  print_display(display);
  bool wait = true;
  while (wait) {
    switch (getch()) {
    case 'p':
      wait = false;
      break;
    }
    usleep(50000);
  }
}