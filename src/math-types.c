#include "math-types.h"

point3f_t point3f_add_vec3f(const point3f_t point, const vec3f_t vec) {
  return (point3f_t){
    .x = point.x + vec.x, .y = point.y + vec.y, .z = point.z + vec.z};
}

point3f_t point3f_sub_vec3f(const point3f_t point, const vec3f_t vec) {
  return (point3f_t){
    .x = point.x - vec.x, .y = point.y - vec.y, .z = point.z - vec.z};
}

vec3f_t vec3f_add_vec3f(const vec3f_t lhs, const vec3f_t rhs) {
  return (vec3f_t){.x = lhs.x + rhs.x, .y = lhs.y + rhs.y, .z = lhs.z + rhs.z};
}

vec3f_t vec3f_sub_vec3f(const vec3f_t lhs, const vec3f_t rhs) {
  return (vec3f_t){.x = lhs.x - rhs.x, .y = lhs.y - rhs.y, .z = lhs.z - rhs.z};
}

vec3f_t vec3f_mul_scalar(const vec3f_t vec, const float scale) {
  return (vec3f_t){.x = vec.x * scale, .y = vec.y * scale, .z = vec.z * scale};
}

vec3f_t point3f_sub_point3f(const point3f_t lhs, const point3f_t rhs) {
  return (vec3f_t){.x = lhs.x - rhs.x, .y = lhs.y - rhs.y, .z = lhs.z - rhs.z};
}
