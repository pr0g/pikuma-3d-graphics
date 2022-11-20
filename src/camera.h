#ifndef CAMERA_H
#define CAMERA_H

#include <as-ops.h>

typedef struct camera_t {
  as_point3f pivot;
  as_vec3f offset;
  float pitch;
  float yaw;
} camera_t;

as_mat34f camera_transform(const camera_t* camera);
as_mat34f camera_view(const camera_t* camera);
as_point3f camera_position(const camera_t* camera);
as_mat33f camera_rotation(const camera_t* camera);

#endif // CAMERA_H
