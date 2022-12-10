#include "camera.h"

as_mat34f camera_transform(const camera_t* camera) {
  const as_mat33f pitch = as_mat33f_x_axis_rotation(camera->pitch);
  const as_mat33f yaw = as_mat33f_y_axis_rotation(camera->yaw);
  const as_mat33f pitch_yaw = as_mat33f_mul_mat33f(&yaw, &pitch);
  const as_mat34f pivot = as_mat34f_translation_from_point3f(camera->pivot);
  const as_mat34f pivot_pitch_yaw = as_mat34f_mul_mat33f(&pivot, &pitch_yaw);
  const as_mat34f offset = as_mat34f_translation_from_vec3f(camera->offset);
  return as_mat34f_mul_mat34f(&pivot_pitch_yaw, &offset);
}

as_mat34f camera_view(const camera_t* camera) {
  const as_mat34f transform = camera_transform(camera);
  return as_mat34f_inverse(&transform);
}

as_point3f camera_position(const camera_t* camera) {
  const as_mat34f transform = camera_transform(camera);
  return as_point3f_from_vec3f(as_vec3f_from_mat34f(&transform, 3));
}

as_mat33f camera_rotation(const camera_t* camera) {
  const as_mat34f transform = camera_transform(camera);
  return as_mat33f_from_mat34f(&transform);
}
