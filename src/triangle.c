#include "triangle.h"

as_vec3f calculate_triangle_normal(const triangle_t triangle) {
  const as_point3f a = triangle.vertices[0];
  const as_point3f b = triangle.vertices[1];
  const as_point3f c = triangle.vertices[2];
  const as_vec3f edge_ab = as_vec3f_normalized(as_point3f_sub_point3f(b, a));
  const as_vec3f edge_ac = as_vec3f_normalized(as_point3f_sub_point3f(c, a));
  return as_vec3f_normalized(as_vec3f_cross_vec3f(edge_ab, edge_ac));
}
