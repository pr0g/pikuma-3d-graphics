#ifndef MESH_H
#define MESH_H

#include <as-ops.h>
#include "texture.h"
#include "triangle.h"

typedef struct mesh_t {
  as_point3f* vertices;
  tex2f_t* uvs;
  face_t* faces;
} mesh_t;

typedef struct model_t {
  mesh_t mesh;
  texture_t texture;
  as_vec3f rotation;
  as_vec3f scale;
  as_vec3f translation;
} model_t;

model_t load_obj_mesh(const char* mesh_path);
model_t load_obj_mesh_with_png_texture(
  const char* mesh_path, const char* texture_path);

#endif // MESH_H
