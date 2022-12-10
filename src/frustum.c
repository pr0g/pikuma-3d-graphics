#include "frustum.h"

#include <math.h>

frustum_planes_t build_frustum_planes(
  const float aspect_ratio,
  const float vertical_fov,
  const float near,
  const float far) {
  const float half_vertical_fov = vertical_fov * 0.5f;
  const float half_horizontal_fov =
    atanf(aspect_ratio * tanf(half_vertical_fov));
  return (frustum_planes_t){
    .planes = {
      [frustum_plane_left] =
        {.normal =
           as_vec3f_rotate_y_axis(as_vec3f_x_axis(), -half_horizontal_fov)},
      [frustum_plane_right] =
        {.normal = as_vec3f_rotate_y_axis(
           as_vec3f_mul_float(as_vec3f_x_axis(), -1.0f), half_horizontal_fov)},
      [frustum_plane_top] =
        {.normal = as_vec3f_rotate_x_axis(
           as_vec3f_mul_float(as_vec3f_y_axis(), -1.0f), -half_vertical_fov)},
      [frustum_plane_bottom] =
        {.normal =
           as_vec3f_rotate_x_axis(as_vec3f_y_axis(), half_vertical_fov)},
      [frustum_plane_near] =
        {.normal = as_vec3f_z_axis(), .point = (as_point3f){.z = near}},
      [frustum_plane_far] =
        {.normal = as_vec3f_mul_float(as_vec3f_z_axis(), -1.0f),
         .point = (as_point3f){.z = far}},
    }};
}
