#ifndef MESH_H
#define MESH_H

#include "math-types.h"
#include "triangle.h"

#define CubeVertexCount 8
#define CubeFaceCount 12

// extern const point3f_t g_cube_vertices[];
// extern const face_t g_cube_faces[];

typedef struct mesh_t {
  point3f_t* vertices;
  face_t* faces;
} mesh_t;

typedef struct model_t {
  mesh_t mesh;
  vec3f_t rotation;
} model_t;

extern model_t g_model;

void load_cube_mesh_data(void);

#endif
