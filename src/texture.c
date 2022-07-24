#include "texture.h"

#include <math.h>
#include <stdio.h>

tex2f_t tex2f_mix(const tex2f_t begin, const tex2f_t end, const float t) {
  return (tex2f_t){.u = mixf(begin.u, end.u, t), .v = mixf(begin.v, end.v, t)};
}

tex2f_t tex2f_div_scalar(const tex2f_t tex, const float scale) {
  return (tex2f_t){tex.u / scale, tex.v / scale};
}

vec3f_t vec3f_from_barycentric_coords(
  const barycentric_coords_t barycentric_coords) {
  return (vec3f_t){
    barycentric_coords.alpha,
    barycentric_coords.beta,
    barycentric_coords.gamma};
}

barycentric_coords_t calculate_barycentric_coordinates(
  const point2i_t a, const point2i_t b, const point2i_t c, const point2i_t p) {
  const vec2i_t ab = point2i_sub_point2i(b, a);
  const vec2i_t bc = point2i_sub_point2i(c, b);
  const vec2i_t ac = point2i_sub_point2i(c, a);
  const vec2i_t ap = point2i_sub_point2i(p, a);
  const vec2i_t bp = point2i_sub_point2i(p, b);
  const float triangle_area =
    vec2f_wedge_vec2f(vec2f_from_vec2i(ab), vec2f_from_vec2i(ac));
  const float alpha =
    vec2f_wedge_vec2f(vec2f_from_vec2i(bc), vec2f_from_vec2i(bp))
    / triangle_area;
  const float beta =
    vec2f_wedge_vec2f(vec2f_from_vec2i(ap), vec2f_from_vec2i(ac))
    / triangle_area;
  const float gamma = 1.0f - alpha - beta;
  return (barycentric_coords_t){.alpha = alpha, .beta = beta, .gamma = gamma};
}

tex2f_t calculate_uv(
  const barycentric_coords_t barycentric_coords,
  const tex2f_t uv0,
  const float w0,
  const tex2f_t uv1,
  const float w1,
  const tex2f_t uv2,
  const float w2) {
  return (tex2f_t){
    .u = (uv0.u / w0) * barycentric_coords.alpha
       + (uv1.u / w1) * barycentric_coords.beta
       + (uv2.u / w2) * barycentric_coords.gamma,
    .v = (uv0.v / w0) * barycentric_coords.alpha
       + (uv1.v / w1) * barycentric_coords.beta
       + (uv2.v / w2) * barycentric_coords.gamma};
}

point2i_t point2i_at_proportion_of_size2i(
  const size2i_t size, const tex2f_t uv) {
  return (point2i_t){
    (int)floorf((float)size.width * uv.u),
    (int)floorf((float)size.height * uv.v)};
}

texture_t load_png_texture(const char* filename) {
  upng_t* texture = upng_new_from_file(filename);
  if (texture != NULL) {
    upng_decode(texture);
    if (upng_get_error(texture) == UPNG_EOK) {
      return (texture_t){
        .png_texture = texture,
        .color_buffer = (uint32_t*)upng_get_buffer(texture),
        .width = (int)upng_get_width(texture),
        .height = (int)upng_get_height(texture)};
    }
  }
  return (texture_t){0};
}
