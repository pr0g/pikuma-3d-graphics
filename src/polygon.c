#include "polygon.h"

#include "array.h"

#include <stddef.h>

polygon_t build_polygon_from_triangle(const triangle_t triangle) {
  polygon_t polygon = (polygon_t){.vertices = NULL, .uvs = NULL};
  for (int v = 0; v < 3; ++v) {
    array_push(polygon.vertices, triangle.vertices[v]);
  }
  return polygon;
}

polygon_t build_polygon_from_uv_triangle(const uv_triangle_t triangle) {
  polygon_t polygon = build_polygon_from_triangle(triangle.triangle);
  for (int v = 0; v < 3; ++v) {
    array_push(polygon.uvs, triangle.uvs[v]);
  }
  return polygon;
}

static void clip_polygon_against_plane(
  polygon_t* polygon, const as_plane plane) {
  const int vertex_count = array_length(polygon->vertices);
  if (vertex_count == 0) {
    return;
  }

  const as_point3f* current_vertex = &polygon->vertices[0];
  const as_point3f* previous_vertex = &polygon->vertices[vertex_count - 1];
  const tex2f_t* current_uv = &polygon->uvs[0];
  const tex2f_t* previous_uv = &polygon->uvs[vertex_count - 1];

  float previous_dot = as_vec3f_dot_vec3f(
    as_point3f_sub_point3f(*previous_vertex, plane.point), plane.normal);

  tex2f_t* inside_uvs = NULL;
  as_point3f* inside_vertices = NULL;
  while (current_vertex
         != &polygon->vertices[array_length(polygon->vertices)]) {
    const float current_dot = as_vec3f_dot_vec3f(
      as_point3f_sub_point3f(*current_vertex, plane.point), plane.normal);
    // if we changed from inside to outside
    if (current_dot * previous_dot < 0.0f) {
      const float t = previous_dot / (previous_dot - current_dot);
      const as_point3f intersection_point =
        as_point3f_mix(*previous_vertex, *current_vertex, t);
      const tex2f_t intersection_uv = tex2f_mix(*previous_uv, *current_uv, t);
      array_push(inside_vertices, intersection_point);
      array_push(inside_uvs, intersection_uv);
    }
    if (current_dot > 0.0f) {
      array_push(inside_vertices, *current_vertex);
      array_push(inside_uvs, *current_uv);
    }
    // move to next vertex
    previous_dot = current_dot;
    previous_uv = current_uv;
    previous_vertex = current_vertex;
    current_uv++;
    current_vertex++;
  }
  // free existing vertices and replace with new vertices
  array_free(polygon->vertices);
  array_free(polygon->uvs);
  polygon->vertices = inside_vertices;
  polygon->uvs = inside_uvs;
}

void clip_polygon_against_frustum(
  polygon_t* polygon, const frustum_planes_t frustum_planes) {
  for (int plane_index = 0; plane_index < FrustumPlaneCount; ++plane_index) {
    clip_polygon_against_plane(polygon, frustum_planes.planes[plane_index]);
  }
}

triangle_t* triangles_from_polygon(const polygon_t polygon) {
  triangle_t* triangles = NULL;
  const int triangle_count = array_length(polygon.vertices) - 2;
  for (int v = 0; v < triangle_count; ++v) {
    const triangle_t triangle = (triangle_t){
      .vertices = {
        [0] = polygon.vertices[0],
        [1] = polygon.vertices[v + 1],
        [2] = polygon.vertices[v + 2]}};
    array_push(triangles, triangle);
  }
  return triangles;
}

uv_triangle_t* uv_triangles_from_polygon(const polygon_t polygon) {
  uv_triangle_t* uv_triangles = NULL;
  triangle_t* triangles = triangles_from_polygon(polygon);
  const int triangle_count = array_length(triangles);
  for (int v = 0; v < triangle_count; ++v) {
    const uv_triangle_t uv_triangle = (uv_triangle_t){
      .triangle = triangles[v],
      .uvs = {
        [0] = polygon.uvs[0],
        [1] = polygon.uvs[v + 1],
        [2] = polygon.uvs[v + 2]}};
    array_push(uv_triangles, uv_triangle);
  }
  array_free(triangles);
  return uv_triangles;
}
