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
  const int vertex_count = array_length(polygon->vertices);
  if (vertex_count == 0) {
    return;
  }

  const point3f_t* current_vertex = &polygon->vertices[0];
  const point3f_t* previous_vertex = &polygon->vertices[vertex_count - 1];

  float previous_dot = vec3f_dot_vec3f(
    point3f_sub_point3f(*previous_vertex, plane.point), plane.normal); // q2

  point3f_t* inside_vertices = NULL;
  while (current_vertex
         != &polygon->vertices[array_length(polygon->vertices)]) {
    const float current_dot = vec3f_dot_vec3f(
      point3f_sub_point3f(*current_vertex, plane.point), plane.normal); // q1
    // if we changed from inside to outside
    if (current_dot * previous_dot < 0.0f) {
      const float t = previous_dot / (previous_dot - current_dot);
      const point3f_t intersection_point =
        point3f_mix(*previous_vertex, *current_vertex, t);
      array_push(inside_vertices, intersection_point);
    }
    if (current_dot > 0.0f) {
      array_push(inside_vertices, *current_vertex);
    }
    // move to next vertex
    previous_dot = current_dot;
    previous_vertex = current_vertex;
    current_vertex++;
  }
  // free existing vertices and replace with new vertices
  array_free(polygon->vertices);
  polygon->vertices = inside_vertices;
}

void clip_polygon_against_frustum(
  polygon_t* polygon, const frustum_planes_t frustum_planes) {
  for (int plane_index = 0; plane_index < FrustumPlaneCount; ++plane_index) {
    clip_polygon_against_plane(polygon, frustum_planes.planes[plane_index]);
  }
}
