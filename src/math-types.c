#include "math-types.h"

#include <math.h>

const float k_pi = 3.14159265358979323846f;
const float k_half_pi = 1.57079632679489661923f;
const float k_two_pi = 6.28318530717958647692f;
const float k_tau = 6.28318530717958647692f;

void swapf(float* lhs, float* rhs) {
  float temp = *lhs;
  *lhs = *rhs;
  *rhs = temp;
}

void swapi(int* lhs, int* rhs) {
  int temp = *lhs;
  *lhs = *rhs;
  *rhs = temp;
}

static int mat_rc(const int r, const int c, const int d) {
  return r * d + c;
}

int mat22_rc(const int r, const int c) {
  return mat_rc(r, c, 2);
}

int mat33_rc(const int r, const int c) {
  return mat_rc(r, c, 3);
}

int mat44_rc(const int r, const int c) {
  return mat_rc(r, c, 4);
}

float radians_from_degrees(const float degrees) {
  return degrees * (k_pi / 180.0f);
}

float degrees_from_radians(const float radians) {
  return radians * (180.0f / k_pi);
}

mat22f_t mat22f_identity(void) {
  return mat22f_uniform_scale_from_float(1.0f);
}

mat33f_t mat33f_identity(void) {
  return mat33f_uniform_scale_from_float(1.0f);
}

mat44f_t mat44f_identity(void) {
  return (mat44f_t){.elem = {[0] = 1.0f, [5] = 1.0f, [10] = 1.0f, [15] = 1.0f}};
}

mat34f_t mat34f_identity(void) {
  return (mat34f_t){.elem = {[0] = 1.0f, [5] = 1.0f, [10] = 1.0f}};
}

mat22f_t mat22f_uniform_scale_from_float(const float scale) {
  return mat22f_scale_from_floats(scale, scale);
}

mat22f_t mat22f_scale_from_floats(const float scale_x, const float scale_y) {
  return (mat22f_t){.elem = {[0] = scale_x, [3] = scale_y}};
}

mat22f_t mat22f_scale_from_vec2f(const vec2f_t scale_xy) {
  return mat22f_scale_from_floats(scale_xy.x, scale_xy.y);
}

mat33f_t mat33f_uniform_scale_from_float(const float scale) {
  return mat33f_scale_from_floats(scale, scale, scale);
}

mat33f_t mat33f_scale_from_floats(
  const float scale_x, const float scale_y, const float scale_z) {
  return (mat33f_t){.elem = {[0] = scale_x, [4] = scale_y, [8] = scale_z}};
}

mat33f_t mat33f_scale_from_vec3f(const vec3f_t scale_xyz) {
  return mat33f_scale_from_floats(scale_xyz.x, scale_xyz.y, scale_xyz.z);
}

mat33f_t mat33f_x_rotation_from_float(const float rotation_radians) {
  const float cos_rotation = cosf(rotation_radians);
  const float sin_rotation = sinf(rotation_radians);
  return (mat33f_t){
    .elem = {
      [0] = 1.0f,
      [4] = cos_rotation,
      [5] = -sin_rotation,
      [7] = sin_rotation,
      [8] = cos_rotation}};
}

mat33f_t mat33f_y_rotation_from_float(const float rotation_radians) {
  const float cos_rotation = cosf(rotation_radians);
  const float sin_rotation = sinf(rotation_radians);
  return (mat33f_t){
    .elem = {
      [0] = cos_rotation,
      [2] = sin_rotation,
      [4] = 1.0f,
      [6] = -sin_rotation,
      [8] = cos_rotation}};
}

mat33f_t mat33f_z_rotation_from_float(const float rotation_radians) {
  const float cos_rotation = cosf(rotation_radians);
  const float sin_rotation = sinf(rotation_radians);
  return (mat33f_t){
    .elem = {
      [0] = cos_rotation,
      [1] = -sin_rotation,
      [3] = sin_rotation,
      [4] = cos_rotation,
      [8] = 1.0f}};
}

mat34f_t mat34f_translation_from_floats(
  const float translation_x,
  const float translation_y,
  const float translation_z) {
  return (mat34f_t){
    .elem = {
      [0] = 1.0f,
      [5] = 1.0f,
      [10] = 1.0f,
      [3] = translation_x,
      [7] = translation_y,
      [11] = translation_z}};
}

mat34f_t mat34f_translation_from_vec3f(const vec3f_t translation) {
  return mat34f_translation_from_floats(
    translation.x, translation.y, translation.z);
}

point2f_t mat22f_multiply_point2f(const mat22f_t mat, const point2f_t point) {
  return (point2f_t){
    .x = mat.elem[0] * point.x + mat.elem[1] * point.y,
    .y = mat.elem[2] * point.x + mat.elem[3] * point.y};
}

point3f_t mat33f_multiply_point3f(const mat33f_t mat, const point3f_t point) {
  return (point3f_t){
    .x = mat.elem[0] * point.x + mat.elem[1] * point.y + mat.elem[2] * point.z,
    .y = mat.elem[3] * point.x + mat.elem[4] * point.y + mat.elem[5] * point.z,
    .z = mat.elem[6] * point.x + mat.elem[7] * point.y + mat.elem[8] * point.z};
}

point3f_t mat34f_multiply_point3f(const mat34f_t mat, const point3f_t point) {
  return (point3f_t){
    .x = mat.elem[0] * point.x + mat.elem[1] * point.y + mat.elem[2] * point.z
       + mat.elem[3],
    .y = mat.elem[4] * point.x + mat.elem[5] * point.y + mat.elem[6] * point.z
       + mat.elem[7],
    .z = mat.elem[8] * point.x + mat.elem[9] * point.y + mat.elem[10] * point.z
       + mat.elem[11]};
}

mat33f_t mat33f_multiply_mat33f(const mat33f_t lhs, const mat33f_t rhs) {
  return (mat33f_t){
    .elem = {
      [0] = lhs.elem[0] * rhs.elem[0] + lhs.elem[1] * rhs.elem[3]
          + lhs.elem[2] * rhs.elem[6],
      [1] = lhs.elem[0] * rhs.elem[1] + lhs.elem[1] * rhs.elem[4]
          + lhs.elem[2] * rhs.elem[7],
      [2] = lhs.elem[0] * rhs.elem[2] + lhs.elem[1] * rhs.elem[5]
          + lhs.elem[2] * rhs.elem[8],
      [3] = lhs.elem[3] * rhs.elem[0] + lhs.elem[4] * rhs.elem[3]
          + lhs.elem[5] * rhs.elem[6],
      [4] = lhs.elem[3] * rhs.elem[1] + lhs.elem[4] * rhs.elem[4]
          + lhs.elem[5] * rhs.elem[7],
      [5] = lhs.elem[3] * rhs.elem[2] + lhs.elem[4] * rhs.elem[5]
          + lhs.elem[5] * rhs.elem[8],
      [6] = lhs.elem[6] * rhs.elem[0] + lhs.elem[7] * rhs.elem[3]
          + lhs.elem[8] * rhs.elem[6],
      [7] = lhs.elem[6] * rhs.elem[1] + lhs.elem[7] * rhs.elem[4]
          + lhs.elem[8] * rhs.elem[7],
      [8] = lhs.elem[6] * rhs.elem[2] + lhs.elem[7] * rhs.elem[5]
          + lhs.elem[8] * rhs.elem[8]}};
}

mat34f_t mat34f_multiply_mat34f(const mat34f_t lhs, const mat34f_t rhs) {
  return (mat34f_t){
    .elem = {
      [0] = lhs.elem[0] * rhs.elem[0] + lhs.elem[1] * rhs.elem[4]
          + lhs.elem[2] * rhs.elem[8],
      [1] = lhs.elem[0] * rhs.elem[1] + lhs.elem[1] * rhs.elem[5]
          + lhs.elem[2] * rhs.elem[9],
      [2] = lhs.elem[0] * rhs.elem[2] + lhs.elem[1] * rhs.elem[6]
          + lhs.elem[2] * rhs.elem[10],
      [3] = lhs.elem[0] * rhs.elem[3] + lhs.elem[1] * rhs.elem[7]
          + lhs.elem[2] * rhs.elem[11] + lhs.elem[3],
      [4] = lhs.elem[4] * rhs.elem[0] + lhs.elem[5] * rhs.elem[4]
          + lhs.elem[6] * rhs.elem[8],
      [5] = lhs.elem[4] * rhs.elem[1] + lhs.elem[5] * rhs.elem[5]
          + lhs.elem[6] * rhs.elem[9],
      [6] = lhs.elem[4] * rhs.elem[2] + lhs.elem[5] * rhs.elem[6]
          + lhs.elem[6] * rhs.elem[10],
      [7] = lhs.elem[4] * rhs.elem[3] + lhs.elem[5] * rhs.elem[7]
          + lhs.elem[6] * rhs.elem[11] + lhs.elem[7],
      [8] = lhs.elem[8] * rhs.elem[0] + lhs.elem[9] * rhs.elem[4]
          + lhs.elem[10] * rhs.elem[8],
      [9] = lhs.elem[8] * rhs.elem[1] + lhs.elem[9] * rhs.elem[5]
          + lhs.elem[10] * rhs.elem[9],
      [10] = lhs.elem[8] * rhs.elem[2] + lhs.elem[9] * rhs.elem[6]
           + lhs.elem[10] * rhs.elem[10],
      [11] = lhs.elem[8] * rhs.elem[3] + lhs.elem[9] * rhs.elem[7]
           + lhs.elem[10] * rhs.elem[11] + lhs.elem[11]}};
}

mat34f_t mat33f_multiply_mat34f(const mat33f_t lhs, const mat34f_t rhs) {
  return (mat34f_t){
    .elem = {
      [0] = lhs.elem[0] * rhs.elem[0] + lhs.elem[1] * rhs.elem[4]
          + lhs.elem[2] * rhs.elem[8],
      [1] = lhs.elem[0] * rhs.elem[1] + lhs.elem[1] * rhs.elem[5]
          + lhs.elem[2] * rhs.elem[9],
      [2] = lhs.elem[0] * rhs.elem[2] + lhs.elem[1] * rhs.elem[6]
          + lhs.elem[2] * rhs.elem[10],
      [3] = lhs.elem[0] * rhs.elem[3] + lhs.elem[1] * rhs.elem[7]
          + lhs.elem[2] * rhs.elem[11],
      [4] = lhs.elem[3] * rhs.elem[0] + lhs.elem[4] * rhs.elem[4]
          + lhs.elem[5] * rhs.elem[8],
      [5] = lhs.elem[3] * rhs.elem[1] + lhs.elem[4] * rhs.elem[5]
          + lhs.elem[5] * rhs.elem[9],
      [6] = lhs.elem[3] * rhs.elem[2] + lhs.elem[4] * rhs.elem[6]
          + lhs.elem[5] * rhs.elem[10],
      [7] = lhs.elem[3] * rhs.elem[3] + lhs.elem[4] * rhs.elem[7]
          + lhs.elem[5] * rhs.elem[11],
      [8] = lhs.elem[6] * rhs.elem[0] + lhs.elem[7] * rhs.elem[4]
          + lhs.elem[8] * rhs.elem[8],
      [9] = lhs.elem[6] * rhs.elem[1] + lhs.elem[7] * rhs.elem[5]
          + lhs.elem[8] * rhs.elem[9],
      [10] = lhs.elem[6] * rhs.elem[2] + lhs.elem[7] * rhs.elem[6]
           + lhs.elem[8] * rhs.elem[10],
      [11] = lhs.elem[6] * rhs.elem[3] + lhs.elem[7] * rhs.elem[7]
           + lhs.elem[8] * rhs.elem[11]}};
}

mat34f_t mat34f_multiply_mat33f(const mat34f_t lhs, const mat33f_t rhs) {
  return (mat34f_t){
    .elem = {
      [0] = lhs.elem[0] * rhs.elem[0] + lhs.elem[1] * rhs.elem[3]
          + lhs.elem[2] * rhs.elem[6],
      [1] = lhs.elem[0] * rhs.elem[1] + lhs.elem[1] * rhs.elem[4]
          + lhs.elem[2] * rhs.elem[7],
      [2] = lhs.elem[0] * rhs.elem[2] + lhs.elem[1] * rhs.elem[5]
          + lhs.elem[2] * rhs.elem[8],
      [3] = lhs.elem[3],
      [4] = lhs.elem[4] * rhs.elem[0] + lhs.elem[5] * rhs.elem[3]
          + lhs.elem[6] * rhs.elem[6],
      [5] = lhs.elem[4] * rhs.elem[1] + lhs.elem[5] * rhs.elem[4]
          + lhs.elem[6] * rhs.elem[7],
      [6] = lhs.elem[4] * rhs.elem[2] + lhs.elem[5] * rhs.elem[5]
          + lhs.elem[6] * rhs.elem[8],
      [7] = lhs.elem[7],
      [8] = lhs.elem[8] * rhs.elem[0] + lhs.elem[9] * rhs.elem[3]
          + lhs.elem[10] * rhs.elem[6],
      [9] = lhs.elem[8] * rhs.elem[1] + lhs.elem[9] * rhs.elem[4]
          + lhs.elem[10] * rhs.elem[7],
      [10] = lhs.elem[8] * rhs.elem[2] + lhs.elem[9] * rhs.elem[5]
           + lhs.elem[10] * rhs.elem[8],
      [11] = lhs.elem[11]}};
}

mat44f_t mat44f_multiply_mat44f(const mat44f_t lhs, const mat44f_t rhs) {
  return (mat44f_t){
    .elem = {
      [0] = lhs.elem[0] * rhs.elem[0] + lhs.elem[1] * rhs.elem[4]
          + lhs.elem[2] * rhs.elem[8] + lhs.elem[3] * rhs.elem[12],
      [1] = lhs.elem[0] * rhs.elem[1] + lhs.elem[1] * rhs.elem[5]
          + lhs.elem[2] * rhs.elem[9] + lhs.elem[3] * rhs.elem[13],
      [2] = lhs.elem[0] * rhs.elem[2] + lhs.elem[1] * rhs.elem[6]
          + lhs.elem[2] * rhs.elem[10] + lhs.elem[3] * rhs.elem[14],
      [3] = lhs.elem[0] * rhs.elem[3] + lhs.elem[1] * rhs.elem[7]
          + lhs.elem[2] * rhs.elem[11] + lhs.elem[3] * rhs.elem[15],
      [4] = lhs.elem[4] * rhs.elem[0] + lhs.elem[5] * rhs.elem[4]
          + lhs.elem[6] * rhs.elem[8] + lhs.elem[7] * rhs.elem[12],
      [5] = lhs.elem[4] * rhs.elem[1] + lhs.elem[5] * rhs.elem[5]
          + lhs.elem[6] * rhs.elem[9] + lhs.elem[7] * rhs.elem[13],
      [6] = lhs.elem[4] * rhs.elem[2] + lhs.elem[5] * rhs.elem[6]
          + lhs.elem[6] * rhs.elem[10] + lhs.elem[7] * rhs.elem[14],
      [7] = lhs.elem[4] * rhs.elem[3] + lhs.elem[5] * rhs.elem[7]
          + lhs.elem[6] * rhs.elem[11] + lhs.elem[7] * rhs.elem[15],
      [8] = lhs.elem[8] * rhs.elem[0] + lhs.elem[9] * rhs.elem[4]
          + lhs.elem[10] * rhs.elem[8] + lhs.elem[10] * rhs.elem[12],
      [9] = lhs.elem[8] * rhs.elem[1] + lhs.elem[9] * rhs.elem[5]
          + lhs.elem[10] * rhs.elem[9] + lhs.elem[10] * rhs.elem[13],
      [10] = lhs.elem[8] * rhs.elem[2] + lhs.elem[9] * rhs.elem[6]
           + lhs.elem[10] * rhs.elem[10] + lhs.elem[10] * rhs.elem[14],
      [11] = lhs.elem[8] * rhs.elem[3] + lhs.elem[9] * rhs.elem[7]
           + lhs.elem[10] * rhs.elem[11] + lhs.elem[10] * rhs.elem[15],
      [12] = lhs.elem[12] * rhs.elem[0] + lhs.elem[13] * rhs.elem[4]
           + lhs.elem[14] * rhs.elem[8] + lhs.elem[15] * rhs.elem[12],
      [13] = lhs.elem[12] * rhs.elem[1] + lhs.elem[13] * rhs.elem[5]
           + lhs.elem[14] * rhs.elem[9] + lhs.elem[15] * rhs.elem[13],
      [14] = lhs.elem[12] * rhs.elem[2] + lhs.elem[13] * rhs.elem[6]
           + lhs.elem[14] * rhs.elem[10] + lhs.elem[15] * rhs.elem[14],
      [15] = lhs.elem[12] * rhs.elem[3] + lhs.elem[13] * rhs.elem[7]
           + lhs.elem[14] * rhs.elem[11] + lhs.elem[15] * rhs.elem[15]}};
}

mat44f_t mat44f_perspective_projection(
  const float aspect_ratio,
  const float fov,
  const float near,
  const float far) {
  const float e = 1.0f / tanf(fov * 0.5f);
  return (mat44f_t){
    .elem = {
      [0] = e / aspect_ratio,
      [5] = e,
      [10] = far / (far - near),
      [11] = (-far * near) / (far - near),
      [14] = 1.0f}};
}

point4f_t mat44f_multiply_point4f(const mat44f_t mat, const point4f_t point) {
  return (point4f_t){
    .x = mat.elem[0] * point.x + mat.elem[1] * point.y + mat.elem[2] * point.z
       + mat.elem[3] * point.w,
    .y = mat.elem[4] * point.x + mat.elem[5] * point.y + mat.elem[6] * point.z
       + mat.elem[7] * point.w,
    .z = mat.elem[8] * point.x + mat.elem[9] * point.y + mat.elem[10] * point.z
       + mat.elem[11] * point.w,
    .w = mat.elem[12] * point.x + mat.elem[13] * point.y
       + mat.elem[14] * point.z + mat.elem[15] * point.w};
}

static point4f_t perspective_divide(const point4f_t point) {
  return (point4f_t){
    .x = point.x / point.w,
    .y = point.y / point.w,
    .z = point.z / point.w,
    .w = point.w};
}

point4f_t mat44f_project_point3f(
  const mat44f_t projection, const point3f_t point) {
  return perspective_divide(
    mat44f_multiply_point4f(projection, point4f_from_point3f(point)));
}

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

int mini(const int lhs, const int rhs) {
  if (lhs < rhs) {
    return lhs;
  }
  return rhs;
}

int minf(const float lhs, const float rhs) {
  if (lhs < rhs) {
    return lhs;
  }
  return rhs;
}

point3f_t point3f_from_vec3f(const vec3f_t vec) {
  return (point3f_t){vec.x, vec.y, vec.z};
}

point4f_t point4f_from_point3f(const point3f_t point) {
  return (point4f_t){point.x, point.y, point.z, 1.0f};
}

point2f_t point2f_from_point4f(const point4f_t point) {
  return (point2f_t){point.x, point.y};
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

point2f_t point2f_from_vec2f(const vec2f_t vec) {
  return (point2f_t){vec.x, vec.y};
}

vec2f_t vec2f_from_point2f(const point2f_t point) {
  return (vec2f_t){point.x, point.y};
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

vec3i_t vec3i_add_vec3i(const vec3i_t lhs, const vec3i_t rhs) {
  return (vec3i_t){.x = lhs.x + rhs.x, .y = lhs.y + rhs.y, .z = lhs.z + rhs.z};
}

vec3i_t vec3i_sub_vec3i(const vec3i_t lhs, const vec3i_t rhs) {
  return (vec3i_t){.x = lhs.x - rhs.x, .y = lhs.y - rhs.y, .z = lhs.z - rhs.z};
}

vec3f_t vec3i_mul_scalar(const vec3i_t vec, const float scale) {
  return (vec3f_t){.x = vec.x * scale, .y = vec.y * scale, .z = vec.z * scale};
}

vec3f_t vec3i_div_scalar(const vec3i_t vec, const float scale) {
  return (vec3f_t){.x = vec.x / scale, .y = vec.y / scale, .z = vec.z / scale};
}

float vec3i_length(const vec3i_t vec) {
  return sqrtf((float)(vec.x * vec.x + vec.y * vec.y + vec.z * vec.z));
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

vec3f_t vec3f_div_scalar(const vec3f_t vec, const float scale) {
  return (vec3f_t){.x = vec.x / scale, .y = vec.y / scale, .z = vec.z / scale};
}

float vec3f_length(const vec3f_t vec) {
  return sqrtf(vec3f_dot_vec3f(vec, vec));
}

vec2i_t vec2i_add_vec2i(const vec2i_t lhs, const vec2i_t rhs) {
  return (vec2i_t){.x = lhs.x + rhs.x, .y = lhs.y + rhs.y};
}

vec2i_t vec2i_sub_vec2i(const vec2i_t lhs, const vec2i_t rhs) {
  return (vec2i_t){.x = lhs.x - rhs.x, .y = lhs.y - rhs.y};
}

vec2f_t vec2i_mul_scalar(const vec2i_t vec, const float scale) {
  return (vec2f_t){.x = vec.x * scale, .y = vec.y * scale};
}

vec2f_t vec2i_div_scalar(const vec2i_t vec, const float scale) {
  return (vec2f_t){.x = vec.x / scale, .y = vec.y / scale};
}

float vec2i_length(const vec2i_t vec) {
  return sqrtf((float)(vec.x * vec.x + vec.y * vec.y));
}

vec2f_t vec2f_add_vec2f(const vec2f_t lhs, const vec2f_t rhs) {
  return (vec2f_t){.x = lhs.x + rhs.x, .y = lhs.y + rhs.y};
}

vec2f_t vec2f_sub_vec3f(const vec2f_t lhs, const vec2f_t rhs) {
  return (vec2f_t){.x = lhs.x - rhs.x, .y = lhs.y - rhs.y};
}

vec2f_t vec2f_mul_scalar(const vec2f_t vec, const float scale) {
  return (vec2f_t){.x = vec.x * scale, .y = vec.y * scale};
}

vec2f_t vec2f_div_scalar(const vec2f_t vec, const float scale) {
  return (vec2f_t){.x = vec.x / scale, .y = vec.y / scale};
}

float vec2f_length(const vec2f_t vec) {
  return sqrtf(vec2f_dot_vec2f(vec, vec));
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

vec3f_t vec3f_cross_vec3f(const vec3f_t lhs, const vec3f_t rhs) {
  return (vec3f_t){
    lhs.y * rhs.z - lhs.z * rhs.y,
    lhs.z * rhs.x - lhs.x * rhs.z,
    lhs.x * rhs.y - lhs.y * rhs.x};
}

float vec2f_wedge_vec2f(const vec2f_t lhs, const vec2f_t rhs) {
  return lhs.x * rhs.y - lhs.y * rhs.x;
}

float vec2f_dot_vec2f(const vec2f_t lhs, const vec2f_t rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y;
}

float vec3f_dot_vec3f(const vec3f_t lhs, const vec3f_t rhs) {
  return lhs.x * rhs.x + lhs.y * rhs.y + lhs.z * rhs.z;
}

vec2f_t vec2f_normalized(const vec2f_t vec) {
  return vec2f_div_scalar(vec, vec2f_length(vec));
}

vec3f_t vec3f_normalized(const vec3f_t vec) {
  return vec3f_div_scalar(vec, vec3f_length(vec));
}
