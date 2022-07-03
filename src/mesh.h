#ifndef MESH_H
#define MESH_H

#include "math-types.h"
#include "texture.h"
#include "triangle.h"

#define CubeVertexCount 8
#define CubeFaceCount 12
#define CubeUvCount 4

typedef struct mesh_t {
  point3f_t* vertices;
  tex2f_t* uvs;
  face_t* faces;
} mesh_t;

typedef struct model_t {
  mesh_t mesh;
  vec3f_t rotation;
  vec3f_t scale;
  vec3f_t translation;
} model_t;

extern model_t g_model;

void load_cube_mesh_data(void);
void load_obj_mesh_data(const char* filename);

#endif // MESH_H
