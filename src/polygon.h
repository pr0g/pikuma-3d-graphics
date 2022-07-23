#ifndef POLYGON_H
#define POLYGON_H

#include "frustum.h"
#include "math-types.h"
#include "texture.h"

typedef struct triangle_t {
  point3f_t vertices[3];
} triangle_t;

typedef struct uv_triangle_t {
  triangle_t triangle;
  tex2f_t uvs[3];
} uv_triangle_t;

typedef struct polygon_t {
  point3f_t* vertices; // array
  tex2f_t* uvs; // array
} polygon_t;

polygon_t build_polygon_from_triangle(triangle_t triangle);
polygon_t build_polygon_from_uv_triangle(uv_triangle_t triangle);

void clip_polygon_against_frustum(
  polygon_t* polygon, frustum_planes_t frustum_planes);

triangle_t* triangles_from_polygon(polygon_t polygon);
uv_triangle_t* uv_triangles_from_polygon(polygon_t polygon);

#endif // POLYGON_H
