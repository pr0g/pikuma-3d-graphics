#ifndef TEXTURE_H
#define TEXTURE_H

#include <as-ops.h>
#include "upng/upng.h"

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

typedef struct texture_t {
  upng_t* png_texture;
  uint32_t* color_buffer;
  int width;
  int height;
} texture_t;

tex2f_t tex2f_mix(tex2f_t begin, tex2f_t end, float t);
tex2f_t tex2f_div_scalar(tex2f_t tex, float scale);

as_vec3f vec3f_from_barycentric_coords(barycentric_coords_t barycentric_coords);
as_point2i point2i_at_proportion_of_size2i(as_size2i size, tex2f_t uv);

barycentric_coords_t calculate_barycentric_coordinates(
  as_point2i a, as_point2i b, as_point2i c, as_point2i p);

tex2f_t calculate_uv(
  barycentric_coords_t barycentric_coords,
  tex2f_t uv0,
  float w0,
  tex2f_t uv1,
  float w1,
  tex2f_t uv2,
  float w2);

texture_t load_png_texture(const char* filename);

#endif // TEXTURE_H
