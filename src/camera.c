#include "camera.h"

as_mat34f camera_transform(const camera_t* camera) {
  return as_mat34f_mul_mat34f_v(
    as_mat34f_mul_mat33f_v(
      as_mat34f_translation_from_point3f(camera->pivot),
      as_mat33f_mul_mat33f_v(
        as_mat33f_y_axis_rotation(camera->yaw),
        as_mat33f_x_axis_rotation(camera->pitch))),
    as_mat34f_translation_from_vec3f(camera->offset));
}

as_mat34f camera_view(const camera_t* camera) {
  return as_mat34f_inverse_v(camera_transform(camera));
}

as_point3f camera_position(const camera_t* camera) {
  return as_point3f_from_vec3f(
    as_vec3f_from_mat34f_v(camera_transform(camera), 3));
}

as_mat33f camera_rotation(const camera_t* camera) {
  return as_mat33f_from_mat34f_v(camera_transform(camera));
}
