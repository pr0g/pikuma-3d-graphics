#include "texture.h"

#include <math.h>
#include <stdio.h>

tex2f_t tex2f_mix(const tex2f_t begin, const tex2f_t end, const float t) {
  return (tex2f_t){
    .u = as_mix_float(begin.u, end.u, t), .v = as_mix_float(begin.v, end.v, t)};
}

tex2f_t tex2f_div_scalar(const tex2f_t tex, const float scale) {
  return (tex2f_t){tex.u / scale, tex.v / scale};
}

as_vec3f vec3f_from_barycentric_coords(
  const barycentric_coords_t barycentric_coords) {
  return (as_vec3f){
    barycentric_coords.alpha,
    barycentric_coords.beta,
    barycentric_coords.gamma};
}

barycentric_coords_t calculate_barycentric_coordinates(
  const as_point2i a,
  const as_point2i b,
  const as_point2i c,
  const as_point2i p) {
  const as_vec2i ab = as_point2i_sub_point2i(b, a);
  const as_vec2i bc = as_point2i_sub_point2i(c, b);
  const as_vec2i ac = as_point2i_sub_point2i(c, a);
  const as_vec2i ap = as_point2i_sub_point2i(p, a);
  const as_vec2i bp = as_point2i_sub_point2i(p, b);
  const float triangle_area =
    as_vec2f_wedge_vec2f(as_vec2f_from_vec2i(ab), as_vec2f_from_vec2i(ac));
  const float alpha =
    as_vec2f_wedge_vec2f(as_vec2f_from_vec2i(bc), as_vec2f_from_vec2i(bp))
    / triangle_area;
  const float beta =
    as_vec2f_wedge_vec2f(as_vec2f_from_vec2i(ap), as_vec2f_from_vec2i(ac))
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

as_point2i point2i_at_proportion_of_size2i(
  const as_size2i size, const tex2f_t uv) {
  return (as_point2i){
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
