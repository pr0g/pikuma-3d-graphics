#ifndef CAMERA_H
#define CAMERA_H

#include "math-types.h"

typedef struct camera_t {
  point3f_t pivot;
  vec3f_t offset;
  float pitch;
  float yaw;
} camera_t;

mat34f_t camera_transform(camera_t camera);
mat34f_t camera_view(camera_t camera);
point3f_t camera_position(camera_t camera);
mat33f_t camera_rotation(camera_t camera);

#endif // CAMERA_H
