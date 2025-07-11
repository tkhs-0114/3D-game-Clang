#include "./game.h"
#include <math.h>
void player_update(int key[8], object *player, object *boss) {
  player->frame++;

  switch (player->state) {
  case 2: // Attack
    if (player->frame >= player->frame_length[player->state]) {
      player->frame = 0;
      player->state = 0;
    }
    break;
  case 3: // Dodge
    if (player->frame >= player->frame_length[player->state]) {
      player->frame = 0;
      player->state = 0;
    }
    player->position[0] +=
        0.2 * sin(player->rotation[1] + player->rotation_local[1]);
    player->position[2] +=
        0.2 * cos(player->rotation[1] + player->rotation_local[1]);
    break;
  case 0: // Idle
  case 1: // Walk

    // 常に中央を向く
    // player->rotation[1] = atan2(-1 * player->position[0], -1 *
    // player->position[2]);

    if (key[5]) {
      player->state = 2; // Attack
      player->frame = 0;
      return;
    }
    if (key[6]) {
      player->state = 3; // Dodge
      player->frame = 0;
      return;
    }

    // アニメーションのフレームを進める
    player->frame = player->frame % player->frame_length[player->state];

    if (key[1]) {
      // 前
      player->state = 1;
      player->position[0] += player->move_speed * sin(player->rotation[1]);
      player->position[2] += player->move_speed * cos(player->rotation[1]);
    } else if (key[2]) {
      // 後
      player->state = 1;
      player->rotation_set_local[1] = 3.14f;
      player->position[0] += -player->move_speed * sin(player->rotation[1]);
      player->position[2] += -player->move_speed * cos(player->rotation[1]);
    } else if (key[3]) {
      // 左
      player->state = 1;
      player->rotation_set_local[1] = -1.0f;
      player->position[0] += -player->move_speed * cos(player->rotation[1]);
      player->position[2] += player->move_speed * sin(player->rotation[1]);
    } else if (key[4]) {
      // 右
      player->state = 1;
      player->rotation_set_local[1] = 1.0f;
      player->position[0] += player->move_speed * cos(player->rotation[1]);
      player->position[2] += -player->move_speed * sin(player->rotation[1]);
    } else {
      player->state = 0;
      // ボスに向く
      player->rotation_set[1] =
          atan2(-1 * player->position[0] + boss->position[0],
                -1 * player->position[2] + boss->position[2]);
      player->rotation_set_local[1] = 0.0f;
    }
  }
}