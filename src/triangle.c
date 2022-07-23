#include "triangle.h"

vec3f_t calculate_triangle_normal(const triangle_t triangle) {
  const point3f_t a = triangle.vertices[0];
  const point3f_t b = triangle.vertices[1];
  const point3f_t c = triangle.vertices[2];
  const vec3f_t edge_ab = vec3f_normalized(point3f_sub_point3f(b, a));
  const vec3f_t edge_ac = vec3f_normalized(point3f_sub_point3f(c, a));
  return vec3f_normalized(vec3f_cross_vec3f(edge_ab, edge_ac));
}
