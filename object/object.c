#include "object.h"
// #include "../display/display.h"
// #include "../matrix/matrix.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <stdbool.h>

double init_matrix[4][4] = {{1.0, 0.0, 0.0, 0.0},
                            {0.0, 1.0, 0.0, 0.0},
                            {0.0, 0.0, 1.0, 0.0},
                            {0.0, 0.0, 0.0, 1.0}};

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
  bool changed = false;
  
  // 位置・回転の変更をチェック
  for (int i = 0; i < 3; i++) {
    if (obj->position[i] != obj->prev_position[i] ||
        obj->rotation[i] != obj->prev_rotation[i] ||
        obj->position_local[i] != obj->prev_position_local[i] ||
        obj->rotation_local[i] != obj->prev_rotation_local[i]) {
      changed = true;
      break;
    }
  }
  
  // 変更がない場合は行列計算をスキップ
  if (!changed && !obj->matrix_dirty) {
    return;
  }
  
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
  
  // 前フレームの値を保存
  for (int i = 0; i < 3; i++) {
    obj->prev_position[i] = obj->position[i];
    obj->prev_rotation[i] = obj->rotation[i];
    obj->prev_position_local[i] = obj->position_local[i];
    obj->prev_rotation_local[i] = obj->rotation_local[i];
  }
  obj->matrix_dirty = false;
}
void draw_object(int display[H][W], object *obj, object *camera, int c) {
  mult4x4(camera->matrix, obj->matrix);

  // 事前に全頂点を変換（キャッシュ化）
  static double transformed_vertices[MAX_VERTEX][4];
  static int cached_state = -1, cached_frame = -1;
  static object *cached_obj = NULL;
  static int cached_vertex_count = 0;

  // キャッシュが無効な場合のみ頂点変換を実行
  if (cached_obj != obj || cached_state != obj->state ||
      cached_frame != obj->frame) {
    int vertex_count = 0;
    // 実際に使用される頂点数をカウント（最適化：キャッシュ化）
    if (cached_obj != obj || cached_state != obj->state) {
      for (int i = 0; i < MAX_LINE; i++) {
        if (obj->line[i][0] == 0 && obj->line[i][1] == 0)
          break;
        if (obj->line[i][0] > vertex_count)
          vertex_count = obj->line[i][0];
        if (obj->line[i][1] > vertex_count)
          vertex_count = obj->line[i][1];
      }
      vertex_count++;
      cached_vertex_count = vertex_count;
    } else {
      vertex_count = cached_vertex_count;
    }

    // 必要な頂点のみ変換（メモリアクセス最適化）
    double (*src_vertices)[4] = obj->vertex[obj->state][obj->frame];
    for (int v = 0; v < vertex_count; v++) {
      // 早期Z-クリッピング：変換前にZ座標をチェック
      double local_z = src_vertices[v][2];
      if (local_z > -0.1) { // カメラに近すぎる場合はスキップ
        transformed_vertices[v][2] = -1.0; // 無効マーク
        continue;
      }
      
      // メモリコピー最適化
      transformed_vertices[v][0] = src_vertices[v][0];
      transformed_vertices[v][1] = src_vertices[v][1];
      transformed_vertices[v][2] = src_vertices[v][2];
      transformed_vertices[v][3] = src_vertices[v][3];
      mult4x1(obj->matrix, transformed_vertices[v]);
    }

    cached_obj = obj;
    cached_state = obj->state;
    cached_frame = obj->frame;
  }

  // 線分描画（事前変換済み頂点を使用）
  for (int i = 0; i < MAX_LINE; i++) {
    if (obj->line[i][0] == 0 && obj->line[i][1] == 0)
      break;

    double *v1 = transformed_vertices[obj->line[i][0]];
    double *v2 = transformed_vertices[obj->line[i][1]];

    // 早期Z-クリッピング結果をチェック
    if (v1[2] <= 0.0001 || v2[2] <= 0.0001 || v1[2] == -1.0 || v2[2] == -1.0) {
      continue;
    }

    // 透視投影（最適化：除算の最小化）
    double inv_z1 = 1.0 / v1[2];
    double inv_z2 = 1.0 / v2[2];
    int x1 = (int)(160 * v1[0] * inv_z1) + (W >> 1);
    int y1 = (int)(-120 * v1[1] * inv_z1) + (H >> 1);
    int x2 = (int)(160 * v2[0] * inv_z2) + (W >> 1);
    int y2 = (int)(-120 * v2[1] * inv_z2) + (H >> 1);

    draw_line(display, x1, y1, x2, y2, c);
  }
}
//