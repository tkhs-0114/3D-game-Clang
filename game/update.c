#include "./game.h"
#include <math.h>
void update(int key[7], object *player, object *boss, object *camera) {
  camera->position[0] = player->position[0];
  camera->position[1] = player->position[1];
  camera->position[2] = player->position[2];
  camera->rotation[0] = player->rotation[0];
  camera->rotation[1] = player->rotation[1];
  camera->rotation[2] = player->rotation[2];

  float dx = boss->position[0] - player->position[0];
  float dy = boss->position[1] - player->position[1];
  float dz = boss->position[2] - player->position[2];
  float distance = sqrtf(dx * dx + dy * dy + dz * dz);
  if (player->state == 2 && player->frame == player->attack_frame) {
    if (distance <= player->attack_length) {
      double target = atan2(-1 * player->position[0] + boss->position[0],
                            -1 * player->position[2] + boss->position[2]);
      double angle = player->rotation[1] + player->rotation_local[1] - target;
      if (angle <= player->attack_range) {
        boss->hp -= player->attack_damage;
      }
    }
  }
  if (boss->state == 1 && boss->frame == boss->attack_frame) {
    if (distance <= boss->attack_length && player->state != 3) {
      double target = atan2(-1 * boss->position[0] + player->position[0],
                            -1 * boss->position[2] + player->position[2]);
      double angle = boss->rotation[1] + boss->rotation_local[1] - target;

      // 角度を -π から π の範囲に正規化
      while (angle > 3.14159)
        angle -= 2 * 3.14159;
      while (angle < -3.14159)
        angle += 2 * 3.14159;

      // 角度差の絶対値で判定
      if (fabs(angle) <= boss->attack_range) {
        player->hp -= boss->attack_damage;
      }
    }
  }
  if (boss->state == 2 && boss->frame == boss->attack_frame) {
    if (distance <= boss->attack2_length && player->state != 3) {
      double target = atan2(-1 * boss->position[0] + player->position[0],
                            -1 * boss->position[2] + player->position[2]);
      double angle = boss->rotation[1] + boss->rotation_local[1] - target;

      // 角度を -π から π の範囲に正規化
      while (angle > 3.14159)
        angle -= 2 * 3.14159;
      while (angle < -3.14159)
        angle += 2 * 3.14159;

      // 角度差の絶対値で判定
      if (fabs(angle) <= boss->attack2_range) {
        player->hp -= boss->attack2_damage;
      }
    }
  }
}