#include "math-types.h"

#include <math.h>

point3f_t point3f_from_vec3f(vec3f_t vec) {
  return (point3f_t){vec.x, vec.y, vec.z};
}

vec3f_t vec3f_from_point3f(point3f_t point) {
  return (vec3f_t){point.x, point.y, point.z};
}

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

vec3f_t vec3f_rotate_x(const vec3f_t vec, const float angle) {
  return (vec3f_t){
    .x = vec.x,
    .y = vec.y * cosf(angle) - vec.z * sinf(angle),
    .z = vec.y * sinf(angle) + vec.z * cosf(angle)};
}

vec3f_t vec3f_rotate_y(const vec3f_t vec, const float angle) {
  return (vec3f_t){
    .x = vec.x * cosf(angle) - vec.z * sinf(angle),
    .y = vec.y,
    .z = vec.x * sinf(angle) + vec.z * cosf(angle)};
}

vec3f_t vec3f_rotate_z(const vec3f_t vec, const float angle) {
  return (vec3f_t){
    .x = vec.x * cosf(angle) - vec.y * sinf(angle),
    .y = vec.x * sinf(angle) + vec.y * cosf(angle),
    .z = vec.z};
}

point3f_t point3f_rotate_x(const point3f_t point, const float angle) {
  return point3f_from_vec3f(vec3f_rotate_x(vec3f_from_point3f(point), angle));
}

point3f_t point3f_rotate_y(const point3f_t point, const float angle) {
  return point3f_from_vec3f(vec3f_rotate_y(vec3f_from_point3f(point), angle));
}

point3f_t point3f_rotate_z(const point3f_t point, const float angle) {
  return point3f_from_vec3f(vec3f_rotate_z(vec3f_from_point3f(point), angle));
}
