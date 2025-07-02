#include "object.h"
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

double init_matrix[4][4] = {{1.0, 0.0, 0.0, 0.0},
                            {0.0, 1.0, 0.0, 0.0},
                            {0.0, 0.0, 1.0, 0.0},
                            {0.0, 0.0, 0.0, 1.0}};
// 並列処理用のCPUのコア数
long num_cores = 1; // Will be set at runtime
void init_num_cores() { num_cores = sysconf(_SC_NPROCESSORS_ONLN); }

void load_vertex(char *input_file, double object_v[MAX_VERTEX][4]) {
  FILE *fp = fopen(input_file, "r");
  if (!fp)
    return;

  char line[MAX_LINE];
  int v_count = 0;

  while (fgets(line, sizeof(line), fp)) {
    if (line[0] == 'v' && line[1] == ' ') {
      double x, y, z;
      if (sscanf(line, "v %lf %lf %lf", &x, &y, &z) == 3) {
        object_v[v_count][0] = x;
        object_v[v_count][1] = y;
        object_v[v_count][2] = z;
        object_v[v_count][3] = 1.0; // 同次座標
        v_count++;
        if (v_count >= MAX_VERTEX)
          break;
      }
    }
  }
  fclose(fp);
}
void load_line(char *input_file, int object_l[MAX_LINE][2]) {
  FILE *fp = fopen(input_file, "r");
  if (!fp)
    return;

  char line[MAX_LINE];
  int l_count = 0;

  while (fgets(line, sizeof(line), fp)) {
    if (line[0] == 'f' && line[1] == ' ') {
      int idx[16], n = 0;
      char *token = strtok(line + 2, " \t\r\n");
      while (token && n < 16) {
        int vi = atoi(token);
        if (vi > 0)
          idx[n++] = vi - 1; // 0-based
        token = strtok(NULL, " \t\r\n");
      }
      for (int i = 0; i < n; i++) {
        object_l[l_count][0] = idx[i];
        object_l[l_count][1] = idx[(i + 1) % n];
        l_count++;
        if (l_count >= MAX_LINE)
          break;
      }
      if (l_count >= MAX_LINE)
        break;
    }
  }
  fclose(fp);
}
int load(object *obj, int total_frame, int loaded_frame) {
  int display[H][W];

  for (int i = 0; i < obj->state_length; i++) {
    for (int j = 0; j < obj->frame_length[i]; j++) {
      char filename[64];
      snprintf(filename, sizeof(filename), "%s%04d.obj", obj->file[i], j + 1);
      load_vertex(filename, obj->vertex[i][j]);
      load_line(filename, obj->line);
      loaded_frame++;
      clear_display(display, 1);
      draw_bar(display, 10, 5, (double)loaded_frame / total_frame, 2);
      print_display(display);
      usleep(10000);
    }
  }
  return loaded_frame;
}
void calc_matrix(object *obj) {
  for (int i = 0; i < 3; i++) {
    if (obj->rotation_local[i] < obj->rotation_set_local[i]) {
      obj->rotation_local[i] += obj->rotation_speed_local;
      if (obj->rotation_local[i] > obj->rotation_set_local[i]) {
        obj->rotation_local[i] = obj->rotation_set_local[i];
      }
    } else if (obj->rotation_local[i] > obj->rotation_set_local[i]) {
      obj->rotation_local[i] -= obj->rotation_speed_local;
      if (obj->rotation_local[i] < obj->rotation_set_local[i]) {
        obj->rotation_local[i] = obj->rotation_set_local[i];
      }
    }
  }
  for (int i = 0; i < 3; i++) {
    if (obj->rotation[i] < obj->rotation_set[i]) {
      obj->rotation[i] += obj->rotation_speed;
      if (obj->rotation[i] > obj->rotation_set[i]) {
        obj->rotation[i] = obj->rotation_set[i];
      }
    } else if (obj->rotation[i] > obj->rotation_set[i]) {
      obj->rotation[i] -= obj->rotation_speed;
      if (obj->rotation[i] < obj->rotation_set[i]) {
        obj->rotation[i] = obj->rotation_set[i];
      }
    }
  }

  copy4x4(init_matrix, obj->matrix);
  multRx4(obj->rotation_local, obj->matrix);
  multPx4(obj->position_local, obj->matrix);
  multRx4(obj->rotation, obj->matrix);
  multPx4(obj->position, obj->matrix);
}

// 並列処理用の構造体
typedef struct {
  object *obj;
  int (*display)[W];
  int color;
  int start_line;
  int end_line;
} draw_data;

void draw_objline(void *arg) {
  draw_data *data = (draw_data *)arg;

  for (int i = data->start_line; i < data->end_line; i++) {
    if (data->obj->line[i][0] == 0 && data->obj->line[i][1] == 0)
      break;

    // 頂点を取得して変換
    double v1[4], v2[4];

    // 頂点1をコピーして変換
    v1[0] = data->obj->vertex[data->obj->state][data->obj->frame]
                             [data->obj->line[i][0]][0];
    v1[1] = data->obj->vertex[data->obj->state][data->obj->frame]
                             [data->obj->line[i][0]][1];
    v1[2] = data->obj->vertex[data->obj->state][data->obj->frame]
                             [data->obj->line[i][0]][2];
    v1[3] = data->obj->vertex[data->obj->state][data->obj->frame]
                             [data->obj->line[i][0]][3];
    mult4x1(data->obj->matrix, v1);

    // 頂点2をコピーして変換
    v2[0] = data->obj->vertex[data->obj->state][data->obj->frame]
                             [data->obj->line[i][1]][0];
    v2[1] = data->obj->vertex[data->obj->state][data->obj->frame]
                             [data->obj->line[i][1]][1];
    v2[2] = data->obj->vertex[data->obj->state][data->obj->frame]
                             [data->obj->line[i][1]][2];
    v2[3] = data->obj->vertex[data->obj->state][data->obj->frame]
                             [data->obj->line[i][1]][3];
    mult4x1(data->obj->matrix, v2);

    // Z座標チェック（クリッピング）
    if (v1[2] <= 0.0001 || v2[2] <= 0.0001) {
      continue;
    }

    // 透視投影
    int x1 = (int)(160 * v1[0] / v1[2]) + (W / 2);
    int y1 = (int)(-120 * v1[1] / v1[2]) + (H / 2);
    int x2 = (int)(160 * v2[0] / v2[2]) + (W / 2);
    int y2 = (int)(-120 * v2[1] / v2[2]) + (H / 2);

    draw_line(data->display, x1, y1, x2, y2, data->color);
  }
}

void draw_object(int display[H][W], object *obj, object *camera, int c) {
  mult4x4(camera->matrix, obj->matrix);

  // 実際の線分数を数える
  int total_lines = 0;
  for (int i = 0; i < MAX_LINE; i++) {
    if (obj->line[i][0] == 0 && obj->line[i][1] == 0)
      break;
    total_lines++;
  }

  // 並列処理
  pthread_t *threads = malloc(num_cores * sizeof(pthread_t));
  draw_data *thread_data = malloc(num_cores * sizeof(draw_data));

  if (!threads || !thread_data) {
    free(threads);
    free(thread_data);
    return;
  }

  // 各スレッドに線分範囲を割り当て
  int lines_per_thread = total_lines / num_cores;
  int remaining_lines = total_lines % num_cores;

  for (int i = 0; i < num_cores; i++) {
    thread_data[i].obj = obj;
    thread_data[i].display = display;
    thread_data[i].color = c;

    // 線分範囲の計算
    thread_data[i].start_line = i * lines_per_thread;
    thread_data[i].end_line = (i + 1) * lines_per_thread;

    // 余りの線分を最後のスレッドに追加
    if (i == num_cores - 1) {
      thread_data[i].end_line += remaining_lines;
    }

    if (pthread_create(&threads[i], NULL, (void *(*)(void *))draw_objline,
                       &thread_data[i]) != 0) {
      perror("Failed to create thread");
      // エラー時は作成済みスレッドの終了を待つ
      for (int j = 0; j < i; j++) {
        pthread_join(threads[j], NULL);
      }
      free(threads);
      free(thread_data);
      return;
    }
  }

  // すべてのスレッドの終了を待つ
  for (int i = 0; i < num_cores; i++) {
    pthread_join(threads[i], NULL);
  }

  // リソースを解放
  free(threads);
  free(thread_data);
}
//