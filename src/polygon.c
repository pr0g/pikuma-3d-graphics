#include "polygon.h"

#include "array.h"

#include <stddef.h>

polygon_t build_polygon_from_triangle(const triangle_t triangle) {
  polygon_t polygon = (polygon_t){.vertices = NULL};
  for (int v = 0; v < 3; ++v) {
    array_push(polygon.vertices, triangle.vertices[v]);
  }
  return polygon;
}

static void clip_polygon_against_plane(
  polygon_t* polygon, const plane_t plane) {
  // todo
}

void clip_polygon_against_frustum(
  polygon_t* polygon, const frustum_planes_t frustum_planes) {
  for (int plane_index = 0; plane_index < FrustumPlaneCount; ++plane_index) {
    clip_polygon_against_plane(polygon, frustum_planes.planes[plane_index]);
  }
}
