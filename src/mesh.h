#ifndef MESH_H
#define MESH_H

#include "math-types.h"
#include "texture.h"
#include "triangle.h"

typedef struct mesh_t {
  point3f_t* vertices;
  tex2f_t* uvs;
  face_t* faces;
} mesh_t;

typedef struct model_t {
  mesh_t mesh;
  texture_t texture;
  vec3f_t rotation;
  vec3f_t scale;
  vec3f_t translation;
} model_t;

model_t load_obj_mesh(const char* mesh_path);
model_t load_obj_mesh_with_png_texture(
  const char* mesh_path, const char* texture_path);

#endif // MESH_H
