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

void load_obj_mesh_data(const char* filename) {
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
    } else if (strncmp(line, "vt ", 3) == 0) {
      line += 3;
      char* token = strtok(line, separator);
      tex2f_t uv = {};
      float* uvs[] = {[0] = &uv.u, [1] = &uv.v};
      int i = 0;
      while (token != NULL) {
        *uvs[i++] = atof(token);
        token = strtok(NULL, separator);
      }
      array_push(g_model.mesh.uvs, uv);
    } else if (strncmp(line, "f ", 2) == 0) {
      line += 2;
      char* token = strtok(line, separator);
      face_t face = {};
      int v = 0;
      int uv = 0;
      while (token != NULL) {
        {
          const char* slash = strchr(token, '/');
          const int len = slash - token;
          char temp[32];
          memcpy(temp, token, len);
          temp[len] = '\0';
          face.vert_indices[v++] = atoi(temp);
          token += len + 1;
        }
        {
          const char* slash = strchr(token, '/');
          const int len = slash - token;
          char temp[32];
          memcpy(temp, token, len);
          temp[len] = '\0';
          face.uv_indices[uv++] = atoi(temp);
        }
        token = strtok(NULL, separator);
      }
      array_push(g_model.mesh.faces, face);
    }
  }
  fclose(file);
}
