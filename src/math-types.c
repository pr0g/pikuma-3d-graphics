#include "math-types.h"

#include <math.h>

int clampi(const int value, const int min, const int max) {
  if (value < min) {
    return min;
  }
  if (value > max) {
    return max;
  }
  return value;
}

float clampf(float value, float min, float max) {
  if (value < min) {
    return min;
  }
  if (value > max) {
    return max;
  }
  return value;
}

float maxf(const float lhs, const float rhs) {
  if (lhs > rhs) {
    return lhs;
  }
  return rhs;
}

int maxi(const int lhs, const int rhs) {
  if (lhs > rhs) {
    return lhs;
  }
  return rhs;
}

point3f_t point3f_from_vec3f(const vec3f_t vec) {
  return (point3f_t){vec.x, vec.y, vec.z};
}

point3f_t point3f_from_point3i(const point3i_t point) {
  return (point3f_t){(float)point.x, (float)point.y, (float)point.z};
}

point3i_t point3i_from_point3f(const point3f_t point) {
  return (point3i_t){
    (int)roundf(point.x), (int)roundf(point.y), (int)roundf(point.z)};
}

point2f_t point2f_from_point2i(const point2i_t point) {
  return (point2f_t){(float)point.x, (float)point.y};
}

point2i_t point2i_from_point2f(const point2f_t point) {
  return (point2i_t){(int)roundf(point.x), (int)roundf(point.y)};
}

vec3f_t vec3f_from_point3f(const point3f_t point) {
  return (vec3f_t){point.x, point.y, point.z};
}

vec2f_t vec2f_from_vec2i(const vec2i_t vec) {
  return (vec2f_t){(float)vec.x, (float)vec.y};
}

vec2i_t vec2i_from_vec2f(const vec2f_t vec) {
  return (vec2i_t){(int)roundf(vec.x), (int)roundf(vec.y)};
}

vec3f_t vec3f_from_vec3i(const vec3i_t vec) {
  return (vec3f_t){(float)vec.x, (float)vec.y, (float)vec.z};
}

vec3i_t vec3i_from_vec3f(const vec3f_t vec) {
  return (vec3i_t){(int)roundf(vec.x), (int)roundf(vec.y), (int)roundf(vec.z)};
}

vec2i_t vec2i_add_vec2i(const vec2i_t lhs, const vec2i_t rhs) {
  return (vec2i_t){lhs.x + rhs.x, lhs.y + rhs.y};
}

point2i_t point2i_add_vec2i(const point2i_t point, const vec2i_t vec) {
  return (point2i_t){point.x + vec.x, point.y + vec.y};
}

vec2i_t point2i_sub_point2i(const point2i_t lhs, const point2i_t rhs) {
  return (vec2i_t){lhs.x - rhs.x, lhs.y - rhs.y};
}

point2f_t point2f_add_vec2f(const point2f_t point, const vec2f_t vec) {
  return (point2f_t){point.x + vec.x, point.y + vec.y};
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

vec2f_t vec2i_div_real(const vec2i_t vec, const float real) {
  return (vec2f_t){vec.x / real, vec.y / real};
}
