#include "./game.h"
#include <math.h>
#include <stdlib.h>
int wait_frame = 30;
void boss_update(int key[7], object *boss, object *player) {
  boss->frame++;
  wait_frame--;
  switch (boss->state) {
  case 1: // Attack1
  case 2: // Attack2
    if (boss->frame >= boss->frame_length[boss->state]) {
      boss->frame = 0;
      boss->state = 0;
      wait_frame = 20 + rand() % 40; // Random wait
    }
    break;
  case 0: // Idle
    boss->frame = boss->frame % boss->frame_length[boss->state];
    if (wait_frame <= 0) {
      boss->state = 1;
      boss->frame = 0;
      // ランダムの範囲で攻撃とプレイヤーを探すの割合を変える
      switch (rand() % 6) {
      case 1: // Attack1
        boss->state = 1;
        break;
      case 2: // Attack2
        boss->state = 2;
        break;
      default: // Look for player
        boss->state = 0;
        boss->rotation_set[1] =
            atan2(-1 * boss->position[0] + player->position[0],
                  -1 * boss->position[2] + player->position[2]);
        wait_frame = 20 + rand() % 40; // Random wait
        break;
      }
    }
    break;
  }
};