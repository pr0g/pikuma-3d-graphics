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
  point3f_t* inside_vertices = NULL;

  point3f_t* current_vertex = &polygon->vertices[0];
  point3f_t* const last_vertex =
    &polygon->vertices[array_length(&polygon->vertices) - 1];
  point3f_t* previous_vertex = last_vertex;

  float previous_dot = vec3f_dot_vec3f(
    point3f_sub_point3f(*previous_vertex, plane.point), plane.normal); // q2

  while (current_vertex != last_vertex) {
    const float current_dot = vec3f_dot_vec3f(
      point3f_sub_point3f(*current_vertex, plane.point), plane.normal); // q1
    // if we changed from inside to outside
    if (current_dot * previous_dot < 0.0f) {
      point3f_t intersection_point = {}; // // calculate intersection point
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
