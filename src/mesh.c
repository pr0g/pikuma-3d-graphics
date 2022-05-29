#include "mesh.h"

point3f_t g_mesh_vertices[] = { // cube
  {.x = -1.0f, .y = -1.0f, .z = -1.0f },
  {.x = -1.0f, .y =  1.0f, .z = -1.0f },
  {.x =  1.0f, .y =  1.0f, .z = -1.0f },
  {.x =  1.0f, .y = -1.0f, .z = -1.0f },
  {.x =  1.0f, .y =  1.0f, .z =  1.0f },
  {.x =  1.0f, .y = -1.0f, .z =  1.0f },
  {.x = -1.0f, .y =  1.0f, .z =  1.0f },
  {.x = -1.0f, .y = -1.0f, .z =  1.0f }
};

face_t g_mesh_faces[] = {
  // front
  {.a = 1, .b = 2, .c = 3},
  {.a = 1, .b = 3, .c = 4},
  // right
  {.a = 4, .b = 3, .c = 5},
  {.a = 4, .b = 5, .c = 6},
  // back
  {.a = 6, .b = 5, .c = 7},
  {.a = 6, .b = 7, .c = 8},
  // left
  {.a = 8, .b = 7, .c = 2},
  {.a = 8, .b = 2, .c = 1},
  // top
  {.a = 2, .b = 7, .c = 5},
  {.a = 2, .b = 5, .c = 3},
  // bottom
  {.a = 6, .b = 8, .c = 1},
  {.a = 6, .b = 1, .c = 4}
};
