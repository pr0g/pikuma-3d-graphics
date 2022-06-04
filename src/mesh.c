#include "mesh.h"

#include "array.h"

#include <stdio.h>

model_t g_model = {
  (mesh_t){.faces = NULL, .vertices = NULL}, (vec3f_t){0.0f, 0.0f, 0.0f}};

const point3f_t g_cube_vertices[] = { // cube
  {.x = -1.0f, .y = -1.0f, .z = -1.0f},
  {.x = -1.0f, .y = 1.0f, .z = -1.0f},
  {.x = 1.0f, .y = 1.0f, .z = -1.0f},
  {.x = 1.0f, .y = -1.0f, .z = -1.0f},
  {.x = 1.0f, .y = 1.0f, .z = 1.0f},
  {.x = 1.0f, .y = -1.0f, .z = 1.0f},
  {.x = -1.0f, .y = 1.0f, .z = 1.0f},
  {.x = -1.0f, .y = -1.0f, .z = 1.0f}};

const face_t g_cube_faces[] = {
  // front
  {.indices = {1, 2, 3}},
  {.indices = {1, 3, 4}},
  // right
  {.indices = {4, 3, 5}},
  {.indices = {4, 5, 6}},
  // back
  {.indices = {6, 5, 7}},
  {.indices = {6, 7, 8}},
  // left
  {.indices = {8, 7, 2}},
  {.indices = {8, 2, 1}},
  // top
  {.indices = {2, 7, 5}},
  {.indices = {2, 5, 3}},
  // bottom
  {.indices = {6, 8, 1}},
  {.indices = {6, 1, 4}}};

void load_cube_mesh_data(void) {
  for (int i = 0; i < CubeVertexCount; ++i) {
    array_push(g_model.mesh.vertices, g_cube_vertices[i]);
  }

  for (int i = 0; i < CubeFaceCount; ++i) {
    array_push(g_model.mesh.faces, g_cube_faces[i]);
  }
}
