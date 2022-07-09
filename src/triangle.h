#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "math-types.h"
#include "texture.h"

#include <stdint.h>

typedef struct face_t {
  int vert_indices[3];
  int uv_indices[3];
} face_t;

typedef struct projected_vertex_t {
  point2i_t point;
  float z;
  float w;
  tex2f_t uv;
} projected_vertex_t;

typedef struct projected_triangle_t {
  projected_vertex_t vertices[3];
  uint32_t color;
} projected_triangle_t;

#endif // TRIANGLE_H
