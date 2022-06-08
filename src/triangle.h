#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "math-types.h"

typedef struct face_t {
  int indices[3];
} face_t;

typedef struct projected_triangle_t {
  point2i_t points[3];
  float average_depth;
} projected_triangle_t;

#endif
