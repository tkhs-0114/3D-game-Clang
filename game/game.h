#include "../object/object.h"
#include <ncurses.h>
#include <stdio.h>

void game(char *file, char *cmd);
void get_key(int key[8]);
void player_update(int key[8], object *player, object *boss);
void boss_update(int key[8], object *boss, object *player);
void update(int key[8], object *player, object *boss, object *camera);