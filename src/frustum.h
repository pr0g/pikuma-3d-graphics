#ifndef FRUSTUM_H
#define FRUSTUM_H

#include "math-types.h"

#define FrustumPlaneCount 6

typedef enum frustum_plane_e {
  frustum_plane_left,
  frustum_plane_right,
  frustum_plane_top,
  frustum_plane_bottom,
  frustum_plane_near,
  frustum_plane_far
} frustum_plane_e;

typedef struct frustum_planes_t {
  plane_t planes[FrustumPlaneCount];
} frustum_planes_t;

frustum_planes_t build_frustum_planes(
  float aspect_ratio, float vertical_fov, float near, float far);

#endif // FRUSTUM_H
