#include "mesh.h"

#include "array.h"
#include "texture.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

model_t load_obj_mesh(const char* mesh_path) {
  model_t model = (model_t){.scale = (as_vec3f){1.0f, 1.0f, 1.0f}};

  FILE* file = fopen(mesh_path, "r");

  char buffer[128];
  buffer[0] = '\0';

  const char* separator = " ";
  while (fgets(buffer, sizeof buffer, file) != NULL) {
    char* line = buffer;
    if (strncmp(line, "v ", 2) == 0) {
      line += 2;
      char* token = strtok(line, separator);
      as_point3f vertex = {0};
      float* vertices[] = {[0] = &vertex.x, [1] = &vertex.y, [2] = &vertex.z};
      int i = 0;
      while (token != NULL) {
        *vertices[i++] = atof(token);
        token = strtok(NULL, separator);
      }
      array_push(model.mesh.vertices, vertex);
    } else if (strncmp(line, "vt ", 3) == 0) {
      line += 3;
      char* token = strtok(line, separator);
      tex2f_t uv = {0};
      float* uvs[] = {[0] = &uv.u, [1] = &uv.v};
      int i = 0;
      while (token != NULL) {
        *uvs[i++] = atof(token);
        token = strtok(NULL, separator);
      }
      array_push(model.mesh.uvs, uv);
    } else if (strncmp(line, "f ", 2) == 0) {
      line += 2;
      char* token = strtok(line, separator);
      face_t face = {0};
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
      array_push(model.mesh.faces, face);
    }
  }
  fclose(file);
  return model;
}

model_t load_obj_mesh_with_png_texture(
  const char* mesh_path, const char* texture_path) {
  model_t model = load_obj_mesh(mesh_path);
  model.texture = load_png_texture(texture_path);
  return model;
}
