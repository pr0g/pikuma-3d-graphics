#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "math-types.h"

typedef struct {
  int a;
  int b;
  int c;
} face_t;

typedef struct {
  point2i_t points[3];
} projected_triangle_t;

#endif
