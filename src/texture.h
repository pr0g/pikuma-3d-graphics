#ifndef TEXTURE_H
#define TEXTURE_H

#include "math-types.h"

#include <stdint.h>

struct projected_triangle_t;

typedef struct tex2f_t {
  float u;
  float v;
} tex2f_t;

typedef struct barycentric_coords_t {
  float alpha;
  float beta;
  float gamma;
} barycentric_coords_t;

barycentric_coords_t calculate_barycentric_coordinates(
  point2i_t a, point2i_t b, point2i_t c, point2i_t p);

tex2f_t calculate_uv(
  barycentric_coords_t barycentric_coords,
  tex2f_t uv0,
  tex2f_t uv1,
  tex2f_t uv2);

point2i_t point2i_at_proportion_of_size2i(size2i_t size, tex2f_t uv);

int redbrick_texture_width(void);
int redbrick_texture_height(void);
uint32_t* redbrick_texture_as_uint32(void);

#endif // TEXTURE_H
