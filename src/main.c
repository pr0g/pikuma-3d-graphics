#include <SDL.h>

#include "array.h"
#include "display.h"
#include "fps.h"
#include "math-types.h"
#include "mesh.h"

#include <assert.h>

projected_triangle_t* g_triangles_to_render = NULL;
point3f_t g_camera_position = {.x = 0.0f, .y = 0.0f, .z = -5.0f};
int64_t g_previous_frame_time = 0;
Fps g_fps = {.head_ = 0, .tail_ = FpsMaxSamples - 1};

void setup(void) {
  create_color_buffer();
  load_obj_file_data("assets/cube.obj");
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

void wait_to_update(void) {
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
}

void calculate_framerate(void) {
  const int64_t time_window =
    calculate_window(&g_fps, SDL_GetPerformanceCounter());
  if (time_window != -1) {
    const double framerate =
      (double)(FpsMaxSamples - 1)
      / ((double)(time_window) / SDL_GetPerformanceFrequency());
    // fprintf(stderr, "fps: %f\n", framerate);
  }
}

void update(void) {
  wait_to_update();
  calculate_framerate();

  g_model.rotation =
    vec3f_add_vec3f(g_model.rotation, (vec3f_t){0.01f, 0.01f, 0.01f});

  for (int i = 0, face_count = array_length(g_model.mesh.faces); i < face_count;
       ++i) {
    const face_t mesh_face = g_model.mesh.faces[i];
    const point3f_t face_vertices[] = {
      [0] = g_model.mesh.vertices[mesh_face.indices[0] - 1],
      [1] = g_model.mesh.vertices[mesh_face.indices[1] - 1],
      [2] = g_model.mesh.vertices[mesh_face.indices[2] - 1]};

    projected_triangle_t projected_triangle;
    for (int v = 0; v < 3; ++v) {
      const point3f_t rotated_vertex = point3f_rotate_z(
        point3f_rotate_y(
          point3f_rotate_x(face_vertices[v], g_model.rotation.x),
          g_model.rotation.y),
        g_model.rotation.z);
      const point3f_t transformed_vertex = point3f_sub_vec3f(
        rotated_vertex, (vec3f_t){0.0f, 0.0f, g_camera_position.z});
      projected_triangle.points[v] = point2i_add_vec2i(
        projecti(transformed_vertex, 640.0f),
        (vec2i_t){window_width() / 2, window_height() / 2});
    }
    array_push(g_triangles_to_render, projected_triangle);
  }
}

void render(void) {
  renderer_clear();

  for (int i = 0, triangle_count = array_length(g_triangles_to_render);
       i < triangle_count;
       ++i) {
    draw_wire_triangle(g_triangles_to_render[i], 0xff00ffff);
  }

  array_free(g_triangles_to_render);
  g_triangles_to_render = NULL;

  render_color_buffer();
  clear_color_buffer(0xff000000);

  renderer_present();
}

void teardown(void) {
  array_free(g_model.mesh.faces);
  array_free(g_model.mesh.vertices);
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
