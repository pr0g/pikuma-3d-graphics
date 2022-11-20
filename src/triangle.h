#ifndef TRIANGLE_H
#define TRIANGLE_H

#include "texture.h"

#include <as-ops.h>
#include <stdint.h>

typedef struct triangle_t {
  as_point3f vertices[3];
} triangle_t;

typedef struct uv_triangle_t {
  triangle_t triangle;
  tex2f_t uvs[3];
} uv_triangle_t;

typedef struct face_t {
  int vert_indices[3];
  int uv_indices[3];
} face_t;

typedef struct projected_vertex_t {
  as_point2i point;
  float z;
  float w;
  tex2f_t uv;
} projected_vertex_t;

typedef struct projected_triangle_t {
  projected_vertex_t vertices[3];
  uint32_t color;
} projected_triangle_t;

as_vec3f calculate_triangle_normal(triangle_t triangle);

#endif // TRIANGLE_H
