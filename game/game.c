#include "./game.h"
#include "../display/display.h"
#include "../matrix/matrix.h"
#include <sys/time.h>
#include <time.h>
#include <unistd.h>

#include <math.h>

int load_data();
void game_init(char *cmd);
void game_loop();
object player;
object boss;
object camera;
object stage;

// 現在時刻をマイクロ秒で取得
long long get_current_time_us() {
  struct timeval tv;
  gettimeofday(&tv, NULL);
  return tv.tv_sec * 1000000LL + tv.tv_usec;
}

void game(char *file, char *cmd) {
  int total_frame;
  int loaded_frame = 0;

  total_frame = load_data(file);

  loaded_frame = load(&player, total_frame, loaded_frame);
  loaded_frame = load(&boss, total_frame, loaded_frame);
  loaded_frame = load(&stage, total_frame, loaded_frame);
  init_num_cores();
  game_init(cmd);

  game_loop();
}
int load_data(const char *filename) {
  int total_frame = 0;

  player.state_length = 4;
  boss.state_length = 3;
  stage.state_length = 1;

  FILE *fp = fopen(filename, "r");
  if (fp == NULL)
    return 0;
  char buffer[128];
  for (int i = 0; i < player.state_length; i++) {
    double tmp;
    if (fgets(buffer, sizeof(buffer), fp))
      sscanf(buffer, "%s %d %lf", player.file[i], &player.frame_length[i],
             &tmp);
    total_frame += player.frame_length[i];
    switch (i) {
    case 0:
      player.hp = (int)tmp;
      player.max_hp = (int)tmp;
      break;
    case 1:
      player.move_speed = tmp;
      player.rotation_speed = tmp;
      player.rotation_speed_local = tmp;
      break;
    case 2:
      player.attack_frame = tmp;
      break;
    default:
      break;
    }
  }
  for (int i = 0; i < boss.state_length; i++) {
    double tmp;
    if (fgets(buffer, sizeof(buffer), fp))
      sscanf(buffer, "%s %d %lf", boss.file[i], &boss.frame_length[i], &tmp);
    total_frame += boss.frame_length[i];
    switch (i) {
    case 0:
      boss.hp = (int)tmp;
      boss.max_hp = (int)tmp;
      break;
    case 1:
      boss.attack_frame = tmp;
      break;
    case 2:
      boss.attack2_frame = tmp;
    default:
      break;
    }
  }
  for (int i = 0; i < stage.state_length; i++) {
    if (fgets(buffer, sizeof(buffer), fp))
      sscanf(buffer, "%s %d", stage.file[i], &stage.frame_length[i]);
    total_frame += stage.frame_length[i];
  }
  total_frame += 1;
  return total_frame;
}
void game_init(char *cmd) {
  player.position[0] = 0.1;
  player.position[1] = 0;
  player.position[2] = -2;
  player.rotation[0] = 0;
  player.rotation[1] = 0;
  player.rotation[2] = 0;
  player.position_local[0] = 0;
  player.position_local[1] = 0;
  player.position_local[2] = 0;
  player.rotation_local[0] = 0;
  player.rotation_local[1] = 0;
  player.rotation_local[2] = 0;
  player.rotation_set[0] = 0;
  player.rotation_set[1] = 0;
  player.rotation_set[2] = 0;
  player.rotation_set_local[0] = 0;
  player.rotation_set_local[1] = 0;
  player.rotation_set_local[2] = 0;
  player.frame = 0;
  player.state = 0;
  // player.hp
  // player.max_hp
  // player.move_speed
  player.rotation_speed = 0.2;
  player.rotation_speed_local = 0.2;
  player.attack_damage = 10;
  player.attack_range = 1.0;
  player.attack_length = 5.0;
  // player.attack_frame

  boss.position[0] = 0;
  boss.position[1] = 0;
  boss.position[2] = 0;
  boss.rotation[0] = 0;
  boss.rotation[1] = 0;
  boss.rotation[2] = 0;
  boss.position_local[0] = 0;
  boss.position_local[1] = 0;
  boss.position_local[2] = 0;
  boss.rotation_local[0] = 0;
  boss.rotation_local[1] = 0;
  boss.rotation_local[2] = 0;
  boss.rotation_set[0] = 0;
  boss.rotation_set[1] = 3.14;
  boss.rotation_set[2] = 0;
  boss.rotation_set_local[0] = 0;
  boss.rotation_set_local[1] = 0;
  boss.rotation_set_local[2] = 0;
  boss.frame = 0;
  boss.state = 0;
  // boss.hp
  // boss.max_hp
  boss.move_speed = 0.1;
  boss.rotation_speed = 0.1;
  boss.rotation_speed_local = 0.1;
  boss.attack_damage = 1000;
  boss.attack_range = 0.5;
  boss.attack_length = 11;
  // boss.attack_frame
  boss.attack2_damage = 500;
  boss.attack2_range = 2.0;
  boss.attack2_length = 12;
  // boss.attack2_frame

  stage.position[0] = 0;
  stage.position[1] = 0;
  stage.position[2] = 0;
  stage.rotation[0] = 0;
  stage.rotation[1] = 0;
  stage.rotation[2] = 0;
  stage.position_local[0] = 0;
  stage.position_local[1] = 0;
  stage.position_local[2] = 0;
  stage.rotation_local[0] = 0;
  stage.rotation_local[1] = 0;
  stage.rotation_local[2] = 0;
  stage.rotation_set[0] = 0;
  stage.rotation_set[1] = 0;
  stage.rotation_set[2] = 0;
  stage.rotation_set_local[0] = 0;
  stage.rotation_set_local[1] = 0;
  stage.rotation_set_local[2] = 0;
  stage.frame = 0;
  stage.state = 0;

  camera.position[0] = 0;
  camera.position[1] = 0;
  camera.position[2] = 0;
  camera.rotation[0] = 0;
  camera.rotation[1] = 0;
  camera.rotation[2] = 0;
  camera.position_local[0] = 0;
  camera.position_local[1] = 3;
  camera.position_local[2] = -5;
  camera.rotation_local[0] = 0;
  camera.rotation_local[1] = 0;
  camera.rotation_local[2] = 0;
  camera.rotation_set_local[0] = 0;
  camera.rotation_set_local[1] = 0;
  camera.rotation_set_local[2] = 0;
  camera.rotation_set[0] = 0;
  camera.rotation_set[1] = 0;
  camera.rotation_set[2] = 0;

}

void game_loop() {
  int display[H][W];
  int key[8] = {0, 0, 0, 0, 0, 0, 0, 0};
  bool loop = true;
  int time_count = 0;

  // フレームレート制御用の変数
  const long long target_frame_time = 50000; // 50ms = 50000μs (20FPS)
  long long frame_start_time;
  long long frame_end_time;
  long long elapsed_time;
  long long sleep_time;

  while (loop > 0) {
    time_count++;
    frame_start_time = get_current_time_us();

    get_key(key);
    if (key[7]) {
      key[7] = 0; // Reset exit key
      loop = false;
    }

    player_update(key, &player, &boss);
    boss_update(key, &boss, &player);
    update(key, &player, &boss, &camera);
    if (player.hp <= 0 || boss.hp <= 0) {
      loop = false;
    }

    calc_matrix(&player);
    calc_matrix(&boss);
    calc_matrix(&stage);
    calc_matrix(&camera);
    inverse4(camera.matrix);
    clear_display(display, 1);
    draw_object(display, &stage, &camera, 3);
    draw_object(display, &boss, &camera, 2);
    draw_object(display, &player, &camera, 4);

    draw_bar(display, 10, 5, (double)boss.hp / boss.max_hp, 2);

    print_display(display);

    // フレームレート制御
    frame_end_time = get_current_time_us();
    elapsed_time = frame_end_time - frame_start_time;
    sleep_time = target_frame_time - elapsed_time;

    if (sleep_time > 0) {
      usleep(sleep_time);
    }
  }
  if (player.hp <= 0) {
    player.rotation_set_local[0] = 3.14f / 2;
    for (int i = 0; i < 100; i++) {
      get_key(key);
      if (key[7]) {
        key[7] = 0; // Reset exit key
        break;
      }
      calc_matrix(&player);
      calc_matrix(&boss);
      calc_matrix(&stage);
      calc_matrix(&camera);
      inverse4(camera.matrix);
      clear_display(display, 1);
      draw_object(display, &stage, &camera, 3);
      draw_object(display, &boss, &camera, 2);
      draw_object(display, &player, &camera, 4);
      draw_bar(display, 10, 5, (double)boss.hp / boss.max_hp, 2);
      draw_text(display, 60, 30, "YOU DIED", 3, 2);
      char clear_time[16];
      snprintf(clear_time, sizeof(clear_time), "%02d:%02d",
               time_count / 20 / 60, (time_count / 20) % 60);
      draw_text(display, 150, 60, clear_time, 2, 4);
      print_display(display);

      usleep(50000);
    }
  } else if (boss.hp <= 0) {
    boss.rotation_set_local[0] = -3.14f / 2;
    for (int i = 0; i < 100; i++) {
      get_key(key);
      if (key[7]) {
        key[7] = 0; // Reset exit key
        break;
      }
      calc_matrix(&player);
      calc_matrix(&boss);
      calc_matrix(&stage);
      calc_matrix(&camera);
      inverse4(camera.matrix);
      clear_display(display, 1);
      draw_object(display, &stage, &camera, 3);
      draw_object(display, &boss, &camera, 2);
      draw_object(display, &player, &camera, 4);
      draw_bar(display, 10, 5, (double)boss.hp / boss.max_hp, 2);
      draw_text(display, 60, 30, "GAME CLEAR", 2, 3);
      char clear_time[16];
      snprintf(clear_time, sizeof(clear_time), "%02d:%02d",
               time_count / 20 / 60, (time_count / 20) % 60);
      draw_text(display, 150, 60, clear_time, 2, 4);
      print_display(display);

      usleep(50000);
    }
  }
}
