#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "math-types.h"
#include "texture.h"

#include <stdint.h>

typedef struct face_t {
  int vert_indices[3];
  int uv_indices[3];
} face_t;

typedef struct projected_triangle_t {
  point2i_t points[3];
  tex2f_t uvs[3];
  float average_depth;
  uint32_t color;
} projected_triangle_t;

#endif // TRIANGLE_H
