#ifndef MATH_TYPES_H
#define MATH_TYPES_H

typedef struct vec2f_t {
  float x;
  float y;
} vec2f_t;

typedef struct vec3f_t {
  float x;
  float y;
  float z;
} vec3f_t;

typedef struct vec2i_t {
  int x;
  int y;
} vec2i_t;

typedef struct vec3i_t {
  int x;
  int y;
  int z;
} vec3i_t;

typedef struct point2f_t {
  float x;
  float y;
} point2f_t;

typedef struct point3f_t {
  float x;
  float y;
  float z;
} point3f_t;

typedef struct point2i_t {
  int x;
  int y;
} point2i_t;

typedef struct point3i_t {
  int x;
  int y;
  int z;
} point3i_t;

typedef struct size2i_t {
  int width;
  int height;
} size2i_t;

typedef struct rect_t {
  point2i_t pos;
  size2i_t size;
} rect_t;

int clampi(int value, int min, int max);
float maxf(float lhs, float rhs);
int maxi(int lhs, int rhs);

point3f_t point3f_from_vec3f(vec3f_t vec);
point3f_t point3f_from_point3i(point3i_t point);
vec3f_t vec3f_from_point3f(point3f_t point);
vec2f_t vec2f_from_vec2i(vec2i_t vec);
point2f_t point2f_from_point2i(point2i_t point);
point2i_t point2i_from_point2f(point2f_t point);

vec2i_t vec2i_add_vec2i(vec2i_t lhs, vec2i_t rhs);
point2i_t point2i_add_vec2i(point2i_t point, vec2i_t vec);
point2f_t point2f_add_vec2f(point2f_t point, vec2f_t vec);
vec2i_t point2i_sub_point2i(point2i_t lhs, point2i_t rhs);

point3f_t point3f_add_vec3f(point3f_t point, vec3f_t vec);
point3f_t point3f_sub_vec3f(point3f_t point, vec3f_t vec);

vec3f_t vec3f_add_vec3f(vec3f_t lhs, vec3f_t rhs);
vec3f_t vec3f_sub_vec3f(vec3f_t lhs, vec3f_t rhs);
vec3f_t vec3f_mul_scalar(vec3f_t vec, float scale);

vec3f_t point3f_sub_point3f(point3f_t lhs, point3f_t rhs);

vec3f_t vec3f_rotate_x(vec3f_t vec, float angle);
vec3f_t vec3f_rotate_y(vec3f_t vec, float angle);
vec3f_t vec3f_rotate_z(vec3f_t vec, float angle);

point3f_t point3f_rotate_x(point3f_t point, float angle);
point3f_t point3f_rotate_y(point3f_t point, float angle);
point3f_t point3f_rotate_z(point3f_t point, float angle);

vec2f_t vec2i_div_real(vec2i_t vec, float real);

#endif // MATH_TYPES_H
