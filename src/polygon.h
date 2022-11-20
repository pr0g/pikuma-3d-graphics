#ifndef POLYGON_H
#define POLYGON_H

#include "frustum.h"
#include <as-ops.h>
#include "texture.h"
#include "triangle.h"

typedef struct polygon_t {
  as_point3f* vertices; // array
  tex2f_t* uvs; // array
} polygon_t;

polygon_t build_polygon_from_triangle(triangle_t triangle);
polygon_t build_polygon_from_uv_triangle(uv_triangle_t triangle);

void clip_polygon_against_frustum(
  polygon_t* polygon, frustum_planes_t frustum_planes);

triangle_t* triangles_from_polygon(polygon_t polygon);
uv_triangle_t* uv_triangles_from_polygon(polygon_t polygon);

#endif // POLYGON_H
