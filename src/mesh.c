#include "mesh.h"

#include "array.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

model_t g_model = {
  (mesh_t){.faces = NULL, .vertices = NULL},
  .scale = (vec3f_t){1.0f, 1.0f, 1.0f},
  .rotation = (vec3f_t){0.0f, 0.0f, 0.0f},
  .translation = (vec3f_t){0.0f, 0.0f, 5.0f}};

static const point3f_t g_cube_vertices[] = {
  {.x = -1.0f, .y = -1.0f, .z = -1.0f},
  {.x = -1.0f, .y = 1.0f, .z = -1.0f},
  {.x = 1.0f, .y = 1.0f, .z = -1.0f},
  {.x = 1.0f, .y = -1.0f, .z = -1.0f},
  {.x = 1.0f, .y = 1.0f, .z = 1.0f},
  {.x = 1.0f, .y = -1.0f, .z = 1.0f},
  {.x = -1.0f, .y = 1.0f, .z = 1.0f},
  {.x = -1.0f, .y = -1.0f, .z = 1.0f}};

static const tex2f_t g_cube_uvs[] = {
  {.u = 1.0f, .v = 0.0f},
  {.u = 0.0f, .v = 0.0f},
  {.u = 1.0f, .v = 1.0f},
  {.u = 0.0f, .v = 1.0f}};

static const face_t g_cube_faces[] = {
  // front
  {.vert_indices = {1, 2, 3}, .uv_indices = {2, 4, 3}},
  {.vert_indices = {1, 3, 4}, .uv_indices = {2, 3, 1}},
  // right
  {.vert_indices = {4, 3, 5}, .uv_indices = {2, 4, 3}},
  {.vert_indices = {4, 5, 6}, .uv_indices = {2, 3, 1}},
  // back
  {.vert_indices = {6, 5, 7}, .uv_indices = {2, 4, 3}},
  {.vert_indices = {6, 7, 8}, .uv_indices = {2, 3, 1}},
  // left
  {.vert_indices = {8, 7, 2}, .uv_indices = {2, 4, 3}},
  {.vert_indices = {8, 2, 1}, .uv_indices = {2, 3, 1}},
  // top
  {.vert_indices = {2, 7, 5}, .uv_indices = {2, 4, 3}},
  {.vert_indices = {2, 5, 3}, .uv_indices = {2, 3, 1}},
  // bottom
  {.vert_indices = {6, 8, 1}, .uv_indices = {2, 4, 3}},
  {.vert_indices = {6, 1, 4}, .uv_indices = {2, 3, 1}}};

void load_cube_mesh_data(void) {
  for (int i = 0; i < CubeVertexCount; ++i) {
    array_push(g_model.mesh.vertices, g_cube_vertices[i]);
  }

  for (int i = 0; i < CubeFaceCount; ++i) {
    array_push(g_model.mesh.faces, g_cube_faces[i]);
  }

  for (int i = 0; i < CubeUvCount; ++i) {
    array_push(g_model.mesh.uvs, g_cube_uvs[i]);
  }
}

void load_obj_file_data(const char* filename) {
  FILE* file = fopen(filename, "r");

  char buffer[128];
  buffer[0] = '\0';

  const char* separator = " ";
  while (fgets(buffer, sizeof buffer, file) != NULL) {
    char* line = buffer;
    if (strncmp(line, "v ", 2) == 0) {
      line += 2;
      char* token = strtok(line, separator);
      point3f_t vertex = {};
      float* vertices[] = {[0] = &vertex.x, [1] = &vertex.y, [2] = &vertex.z};
      int i = 0;
      while (token != NULL) {
        *vertices[i++] = atof(token);
        token = strtok(NULL, separator);
      }
      array_push(g_model.mesh.vertices, vertex);
    } else if (strncmp(line, "f ", 2) == 0) {
      line += 2;
      char* token = strtok(line, separator);
      face_t face = {};
      int i = 0;
      while (token != NULL) {
        const char* slash = strchr(token, '/');
        const int len = slash - token;
        char temp[32];
        memcpy(temp, token, len);
        temp[len] = '\0';
        face.vert_indices[i++] = atoi(temp);
        token = strtok(NULL, separator);
      }
      array_push(g_model.mesh.faces, face);
    }
  }
  fclose(file);
}
