#include "../object/object.h"
#include <ncurses.h>
#include <stdio.h>

void game();
void get_key(int key[7]);
void player_update(int key[7], object *player, object *boss);
void boss_update(int key[7], object *boss, object *player);
void update(int key[7], object *player, object *boss, object *camera);