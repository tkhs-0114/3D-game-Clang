#include "./game.h"
#include <stdbool.h>
int pre_key_count = 0;
char buf[128];
void apply_key(int key[7]);
int keyToNum(char key) {
  switch (key) {
  case 'q':
    return 7;
  case ' ':
    return 5;
  case 'k':
    return 6;
  case 'w':
    return 1;
  case 's':
    return 2;
  case 'a':
    return 3;
  case 'd':
    return 4;
  default:
    return 0;
  }
}

void get_key(int keys[7]) {
  getstr(buf);

  switch (buf[0]) {
  case 'q':
    keys[7] = 1;
    keys[0] = 7;
    return;
  case ' ':
    keys[5] = 1;
    keys[0] = 5;
    keys[1] = 0;
    keys[2] = 0;
    keys[3] = 0;
    keys[4] = 0;
    keys[6] = 0;
    return;
  case 'k':
    keys[6] = 1;
    keys[0] = 6;
    keys[1] = 0;
    keys[2] = 0;
    keys[3] = 0;
    keys[4] = 0;
    keys[5] = 0;
    return;
  }

  if (keys[0] == 0) {
    pre_key_count = 0;
    apply_key(keys);
  } else {
    if (pre_key_count < 11) {
      pre_key_count++;
    } else {
      if (keys[0] != keyToNum(buf[0])) {
        apply_key(keys);
        pre_key_count = 0;
      }
    }
  }
}
void apply_key(int keys[7]) {
  switch (buf[0]) {
  case 'w':
    keys[1] = 1;
    keys[0] = 1;
    keys[2] = 0;
    break;
  case 's':
    keys[2] = 1;
    keys[0] = 2;
    keys[1] = 0;
    break;
  case 'a':
    keys[3] = 1;
    keys[0] = 3;
    keys[4] = 0;
    break;
  case 'd':
    keys[4] = 1;
    keys[0] = 4;
    keys[3] = 0;
    break;
  default:
    keys[0] = 0;
    keys[1] = 0;
    keys[2] = 0;
    keys[3] = 0;
    keys[4] = 0;
    keys[5] = 0;
    keys[6] = 0;
    keys[7] = 0;
  }
}