#include "matrix.h"
#include <math.h>

void copy4x4(double A[4][4], double B[4][4]) {
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      B[i][j] = A[i][j];
    }
  }
}
void mult4x4(double A[4][4], double B[4][4]) {
  double result[4][4];

  // ループアンローリングによる最適化
  for (int i = 0; i < 4; i++) {
    for (int j = 0; j < 4; j++) {
      result[i][j] = A[i][0] * B[0][j] + A[i][1] * B[1][j] + 
                     A[i][2] * B[2][j] + A[i][3] * B[3][j];
    }
  }
  copy4x4(result, B);
}
void mult4x1(double A[4][4], double B[4]) {
  double result[4];

  // ループアンローリングによる最適化
  result[0] = A[0][0] * B[0] + A[0][1] * B[1] + A[0][2] * B[2] + A[0][3] * B[3];
  result[1] = A[1][0] * B[0] + A[1][1] * B[1] + A[1][2] * B[2] + A[1][3] * B[3];
  result[2] = A[2][0] * B[0] + A[2][1] * B[1] + A[2][2] * B[2] + A[2][3] * B[3];
  result[3] = A[3][0] * B[0] + A[3][1] * B[1] + A[3][2] * B[2] + A[3][3] * B[3];
  
  B[0] = result[0];
  B[1] = result[1];
  B[2] = result[2];
  B[3] = result[3];
}
void inverse4(double A[4][4]) {
  double I[4][4] = {{1, 0, 0, 0}, {0, 1, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
  double A_copy[4][4];

  copy4x4(A, A_copy);

  for (int i = 0; i < 4; i++) {
    if (A_copy[i][i] == 0.0)
      return;

    double pivot = A_copy[i][i];
    for (int j = 0; j < 4; j++) {
      A_copy[i][j] /= pivot;
      I[i][j] /= pivot;
    }

    for (int k = 0; k < 4; k++) {
      if (k == i)
        continue;
      double factor = A_copy[k][i];
      for (int j = 0; j < 4; j++) {
        A_copy[k][j] -= factor * A_copy[i][j];
        I[k][j] -= factor * I[i][j];
      }
    }
  }
  copy4x4(I, A);
  return;
}
void multPx4(double p[3], double B[4][4]) {
  double A[4][4] = {
      {1, 0, 0, p[0]}, {0, 1, 0, p[1]}, {0, 0, 1, p[2]}, {0, 0, 0, 1}};
  mult4x4(A, B);
}
void multRx4(double r[3], double B[4][4]) {
  double cz = cos(r[2]), sz = sin(r[2]);
  double cy = cos(r[1]), sy = sin(r[1]);
  double cx = cos(r[0]), sx = sin(r[0]);

  double Rz[4][4] = {
      {cz, -sz, 0, 0}, {sz, cz, 0, 0}, {0, 0, 1, 0}, {0, 0, 0, 1}};
  double Rx[4][4] = {
      {1, 0, 0, 0}, {0, cx, -sx, 0}, {0, sx, cx, 0}, {0, 0, 0, 1}};
  double Ry[4][4] = {
      {cy, 0, sy, 0}, {0, 1, 0, 0}, {-sy, 0, cy, 0}, {0, 0, 0, 1}};
  mult4x4(Rz, B);
  mult4x4(Rx, B);
  mult4x4(Ry, B);
}
//