#include "camera.h"

mat34f_t camera_transform(const camera_t camera) {
  const mat33f_t pitch = mat33f_x_rotation_from_float(camera.pitch);
  const mat33f_t yaw = mat33f_y_rotation_from_float(camera.yaw);
  return mat34f_multiply_mat34f(
    mat34f_multiply_mat33f(
      mat34f_translation_from_point3f(camera.pivot),
      mat33f_multiply_mat33f(yaw, pitch)),
    mat34f_translation_from_vec3f(camera.offset));
}

mat34f_t camera_view(const camera_t camera) {
  return mat34f_inverse(camera_transform(camera));
}

point3f_t camera_position(const camera_t camera) {
  const mat34f_t transform = camera_transform(camera);
  return point3f_from_vec3f(vec3f_from_mat34f(transform));
}

mat33f_t camera_rotation(const camera_t camera) {
  const mat34f_t transform = camera_transform(camera);
  return mat33f_from_mat34f(transform);
}
