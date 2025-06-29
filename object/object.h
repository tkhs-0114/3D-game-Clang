#include "../display/display.h"
#include "../matrix/matrix.h"
#include <stdbool.h>
#define MAX_VERTEX 1000
#define MAX_LINE 2000
/*
本来4x1行列は double[4][1]とするべきであるが、
今回は書きやすさのため、double[4]と扱う
*/

typedef struct {
  // よく使用されるデータを構造体の先頭に配置（キャッシュ効率向上）
  int state;
  int frame;
  double matrix[4][4];
  double position[3];
  double rotation[3];

  // 行列計算最適化用フラグ
  bool matrix_dirty;
  bool needs_redraw; // 描画更新フラグ
  double prev_position[3];
  double prev_rotation[3];
  double prev_rotation_local[3];
  double prev_position_local[3];

  // 頻度の低いデータ
  double vertex[4][60][MAX_VERTEX][4];
  int line[MAX_LINE][2];
  int frame_length[4];
  int state_length;
  char file[4][32];
  double position_local[3];
  double rotation_local[3];
  double rotation_set[3];
  double rotation_set_local[3];
  int hp;
  int max_hp;
  double move_speed;
  double rotation_speed;
  double rotation_speed_local;
  double attack_damage;
  double attack_range;
  double attack_length;
  double attack_frame;
  double attack2_damage;
  double attack2_range;
  double attack2_length;
  double attack2_frame;
} object; // packed属性を削除してアライメント警告を回避

extern int load(object *obj, int total_frame, int loaded_frame);
extern void calc_matrix(object *obj);
extern void draw_object(int display[H][W], object *obj, object *camera, int c);