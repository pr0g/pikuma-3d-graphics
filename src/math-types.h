#ifndef MATH_TYPES_H
#define MATH_TYPES_H

extern const float k_pi;
extern const float k_half_pi;
extern const float k_two_pi;
extern const float k_tau;

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

typedef struct point4f_t {
  float x;
  float y;
  float z;
  float w;
} point4f_t;

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

typedef struct mat22f_t {
  float elem[4];
} mat22f_t;

typedef struct mat33f_t {
  float elem[9];
} mat33f_t;

typedef struct mat44f_t {
  float elem[16];
} mat44f_t;

typedef struct mat34f_t {
  float elem[12];
} mat34f_t;

typedef struct plane_t {
  point3f_t point;
  vec3f_t normal;
} plane_t;

void swapf(float* lhs, float* rhs);
void swapi(int* lhs, int* rhs);

int clampi(int value, int min, int max);
float clampf(float value, float min, float max);

int maxi(int lhs, int rhs);
float maxf(float lhs, float rhs);

int mini(int lhs, int rhs);
int minf(float lhs, float rhs);

float radians_from_degrees(float degrees);
float degrees_from_radians(float radians);

int mat22_rc(int r, int c);
int mat33_rc(int r, int c);
int mat44_rc(int r, int c);

float mixf(float begin, float end, float t);

point3f_t point3f_mix(point3f_t begin, point3f_t end, float t);
vec3f_t vec3f_mix(vec3f_t begin, vec3f_t end, float t);

mat22f_t mat22f_identity(void);
mat33f_t mat33f_identity(void);
mat34f_t mat34f_identity(void);
mat44f_t mat44f_identity(void);

mat22f_t mat22f_uniform_scale_from_float(float scale);
mat22f_t mat22f_scale_from_floats(float scale_x, float scale_y);
mat22f_t mat22f_scale_from_vec2f(vec2f_t scale_xy);

mat33f_t mat33f_uniform_scale_from_float(float scale);
mat33f_t mat33f_scale_from_floats(float scale_x, float scale_y, float scale_z);
mat33f_t mat33f_scale_from_vec3f(vec3f_t scale_xyz);
mat33f_t mat33f_from_mat34f(mat34f_t mat);
mat33f_t mat33f_transpose(mat33f_t mat);

mat33f_t mat33f_x_rotation_from_float(float rotation_radians);
mat33f_t mat33f_y_rotation_from_float(float rotation_radians);
mat33f_t mat33f_z_rotation_from_float(float rotation_radians);

mat34f_t mat34f_translation_from_floats(
  float translation_x, float translation_y, float translation_z);
mat34f_t mat34f_translation_from_vec3f(vec3f_t translation);
mat34f_t mat34f_translation_from_point3f(point3f_t position);
mat34f_t mat34f_from_mat33f_and_vec3f(mat33f_t rotation, vec3f_t translation);

point2f_t mat22f_multiply_point2f(mat22f_t mat, point2f_t point);
point3f_t mat33f_multiply_point3f(mat33f_t mat, point3f_t point);
point3f_t mat34f_multiply_point3f(mat34f_t mat, point3f_t point);

vec3f_t mat33f_multiply_vec3f(mat33f_t mat, vec3f_t vec);
vec3f_t mat34f_multiply_vec3f(mat34f_t mat, vec3f_t vec);

mat33f_t mat33f_multiply_mat33f(mat33f_t lhs, mat33f_t rhs);
mat34f_t mat34f_multiply_mat34f(mat34f_t lhs, mat34f_t rhs);
mat34f_t mat33f_multiply_mat34f(mat33f_t lhs, mat34f_t rhs);
mat34f_t mat34f_multiply_mat33f(mat34f_t lhs, mat33f_t rhs);
mat44f_t mat44f_multiply_mat44f(mat44f_t lhs, mat44f_t rhs);
mat34f_t mat34f_inverse(mat34f_t mat);

mat44f_t mat44f_perspective_projection(
  float aspect_ratio, float fov, float near, float far);

point4f_t mat44f_multiply_point4f(mat44f_t mat, point4f_t point);
point4f_t mat44f_project_point3f(mat44f_t projection, point3f_t point);

vec2f_t vec2f_from_point2f(point2f_t point);
vec3f_t vec3f_from_point3f(point3f_t point);
point2f_t point2f_from_vec2f(vec2f_t vec);
point2f_t point2f_from_point4f(point4f_t point);
point3f_t point3f_from_vec3f(vec3f_t vec);
point4f_t point4f_from_point3f(point3f_t point);
point4f_t point4f_from_point2f(point2f_t point);

point2f_t point2f_from_point2i(point2i_t point);
point2i_t point2i_from_point2f(point2f_t point);
point3f_t point3f_from_point3i(point3i_t point);
point3i_t point3i_from_point3f(point3f_t point);

vec2f_t vec2f_from_vec2i(vec2i_t vec);
vec2i_t vec2i_from_vec2f(vec2f_t vec);
vec3f_t vec3f_from_vec3i(vec3i_t vec);
vec3i_t vec3i_from_vec3f(vec3f_t vec);
vec3f_t vec3f_from_mat34f(mat34f_t mat);

point2i_t point2i_add_vec2i(point2i_t point, vec2i_t vec);
point2f_t point2f_add_vec2f(point2f_t point, vec2f_t vec);
vec2f_t point2f_sub_point2f(point2f_t lhs, point2f_t rhs);
float point2f_distance_point2f(point2f_t lhs, point2f_t rhs);
vec2i_t point2i_sub_point2i(point2i_t lhs, point2i_t rhs);
float point2i_distance_point2i(point2i_t lhs, point2i_t rhs);

point3f_t point3f_add_vec3f(point3f_t point, vec3f_t vec);
point3f_t point3f_sub_vec3f(point3f_t point, vec3f_t vec);
vec3f_t point3f_sub_point3f(point3f_t lhs, point3f_t rhs);

vec3i_t vec3i_add_vec3i(vec3i_t lhs, vec3i_t rhs);
vec3i_t vec3i_sub_vec3i(vec3i_t lhs, vec3i_t rhs);
vec3f_t vec3i_mul_scalar(vec3i_t vec, float scale);
vec3f_t vec3i_div_scalar(vec3i_t vec, float scale);
float vec3i_length(vec3i_t vec);

vec3f_t vec3f_add_vec3f(vec3f_t lhs, vec3f_t rhs);
vec3f_t vec3f_sub_vec3f(vec3f_t lhs, vec3f_t rhs);
vec3f_t vec3f_mul_scalar(vec3f_t vec, float scale);
vec3f_t vec3f_div_scalar(vec3f_t vec, float scale);
float vec3f_length(vec3f_t vec);

vec2i_t vec2i_add_vec2i(vec2i_t lhs, vec2i_t rhs);
vec2i_t vec2i_sub_vec2i(vec2i_t lhs, vec2i_t rhs);
vec2f_t vec2i_mul_scalar(vec2i_t vec, float scale);
vec2f_t vec2i_div_scalar(vec2i_t vec, float scale);
float vec2i_length(vec2i_t vec);

vec2f_t vec2f_add_vec2f(vec2f_t lhs, vec2f_t rhs);
vec2f_t vec2f_sub_vec2f(vec2f_t lhs, vec2f_t rhs);
vec2f_t vec2f_mul_scalar(vec2f_t vec, float scale);
vec2f_t vec2f_div_scalar(vec2f_t vec, float scale);
float vec2f_length(vec2f_t vec);

vec3f_t vec3f_rotate_x(vec3f_t vec, float angle);
vec3f_t vec3f_rotate_y(vec3f_t vec, float angle);
vec3f_t vec3f_rotate_z(vec3f_t vec, float angle);

point3f_t point3f_rotate_x(point3f_t point, float angle);
point3f_t point3f_rotate_y(point3f_t point, float angle);
point3f_t point3f_rotate_z(point3f_t point, float angle);

vec3f_t vec3f_cross_vec3f(vec3f_t lhs, vec3f_t rhs);
float vec2f_wedge_vec2f(vec2f_t lhs, vec2f_t rhs);

float vec2f_dot_vec2f(vec2f_t lhs, vec2f_t rhs);
float vec3f_dot_vec3f(vec3f_t lhs, vec3f_t rhs);

vec2f_t vec2f_normalized(vec2f_t vec);
vec3f_t vec3f_normalized(vec3f_t vec);

vec3f_t vec3f_x_axis(void);
vec3f_t vec3f_y_axis(void);
vec3f_t vec3f_z_axis(void);

#endif // MATH_TYPES_H
