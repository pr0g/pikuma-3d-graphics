#include <SDL.h>

#include "display.h"
#include "fps.h"
#include "math-types.h"
#include "mesh.h"

#include <assert.h>

projected_triangle_t g_triangles_to_render[MeshFaceCount];

point3f_t g_camera_position = {.x = 0.0f, .y = 0.0f, .z = -5.0f};
vec3f_t g_cube_rotation = {.x = 0.0f, .y = 0.0f, .z = 0.0f};

int64_t g_previous_frame_time = 0;

Fps g_fps = {.head_ = 0, .tail_ = FpsMaxSamples - 1};

void setup(void) {
  create_color_buffer();
}

bool process_input(void) {
  SDL_Event event;
  SDL_PollEvent(&event);
  switch (event.type) {
    case SDL_QUIT:
      return false;
    case SDL_KEYDOWN:
      if (event.key.keysym.sym == SDLK_ESCAPE) {
        return false;
      }
    default:
      break;
  }
  return true;
}

point2f_t projectf(const point3f_t point, const float fov) {
  return (point2f_t){
    .x = (fov * point.x) / point.z, .y = (fov * point.y) / point.z};
}

point2i_t projecti(const point3f_t point, const float fov) {
  const point2f_t projected_point = projectf(point, fov);
  return (point2i_t){
    .x = (int)(roundf(projected_point.x)),
    .y = (int)(roundf(projected_point.y))};
}

double seconds_elapsed(
  const uint64_t old_counter, const uint64_t current_counter) {
  return (current_counter - old_counter)
       / (double)SDL_GetPerformanceFrequency();
}

void update(void) {
  // reference: https://davidgow.net/handmadepenguin/ch18.html
  // seconds elapsed since last update
  const double seconds =
    seconds_elapsed(g_previous_frame_time, SDL_GetPerformanceCounter());
  if (seconds < seconds_per_frame()) {
    // wait for one ms less than delay (due to precision issues)
    const double remainder_s = (double)seconds_per_frame() - seconds;
    // wait 4ms less than actual remainder due to resolution of SDL_Delay
    // (we don't want to delay/sleep too long and get behind)
    const double remainder_pad_s = remainder_s - 0.004;
    const double remainder_pad_ms = remainder_pad_s * 1000.0;
    const double remainder_pad_ms_clamped = maxf(remainder_pad_ms, 0.0f);
    const uint32_t delay = (uint32_t)remainder_pad_ms_clamped;
    SDL_Delay(delay);
    const double seconds_left =
      seconds_elapsed(g_previous_frame_time, SDL_GetPerformanceCounter());
    // check we didn't wait too long and get behind
    assert(seconds_left < seconds_per_frame());
    // busy wait for the remaining time
    while (seconds_elapsed(g_previous_frame_time, SDL_GetPerformanceCounter())
           < seconds_per_frame()) {
      ;
    }
  }
  g_previous_frame_time = SDL_GetPerformanceCounter();

  const int64_t time_window =
    calculateWindow(&g_fps, SDL_GetPerformanceCounter());
  if (time_window != -1) {
    const double framerate =
      (double)(FpsMaxSamples - 1)
      / ((double)(time_window) / SDL_GetPerformanceFrequency());
    // fprintf(stderr, "fps: %f\n", framerate);
  }

  g_cube_rotation.x += 0.01f;
  g_cube_rotation.y += 0.01f;
  g_cube_rotation.z += 0.01f;

  for (int i = 0; i < MeshFaceCount; ++i) {
    face_t mesh_face = g_mesh_faces[i];
    point3f_t face_vertices[] = {
      [0] = g_mesh_vertices[mesh_face.a - 1],
      [1] = g_mesh_vertices[mesh_face.b - 1],
      [2] = g_mesh_vertices[mesh_face.c - 1]};

    projected_triangle_t projected_triangle;
    for (int v = 0; v < 3; ++v) {
      const point3f_t rotated_vertex = point3f_rotate_z(
        point3f_rotate_y(
          point3f_rotate_x(face_vertices[v], g_cube_rotation.x),
          g_cube_rotation.y),
        g_cube_rotation.z);
      const point3f_t transformed_vertex = point3f_sub_vec3f(
        rotated_vertex, (vec3f_t){0.0f, 0.0f, g_camera_position.z});
      projected_triangle.points[v] = point2i_add_vec2i(
        projecti(transformed_vertex, 640.0f),
        (vec2i_t){window_width() / 2, window_height() / 2});
    }
    g_triangles_to_render[i] = projected_triangle;
  }
}

void render(void) {
  renderer_clear();

  for (int i = 0; i < MeshFaceCount; ++i) {
    const projected_triangle_t triangle = g_triangles_to_render[i];
    for (int v = 0; v < 3; ++v) {
      draw_rect(
        &(rect_t){triangle.points[v], (size2i_t){.width = 3, .height = 3}},
        0xffffff00);
    }
  }

  render_color_buffer();
  clear_color_buffer(0xff000000);

  renderer_present();
}

void teardown(void) {
  destroy_color_buffer();
  deinitialize_window();
}

int main(int argc, char** argv) {
  bool is_running = initialize_window();

  setup();

  while (is_running) {
    is_running = process_input();
    update();
    render();
  }

  teardown();

  return 0;
}
