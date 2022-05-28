#include <SDL.h>

#include "display.h"
#include "fps.h"
#include "math-types.h"

#include <assert.h>

const int g_cube_dimension = 9;
const int g_cube_point_count =
  g_cube_dimension * g_cube_dimension * g_cube_dimension;
point3f_t g_cube_points[g_cube_point_count];
point2f_t g_projected_cube_points[g_cube_point_count];

point3f_t g_camera_position = {.x = 0.0f, .y = 0.0f, .z = -5.0f};
vec3f_t g_cube_rotation = {.x = 0.0f, .y = 0.0f, .z = 0.0f};

int64_t g_previous_frame_time = 0;

Fps g_fps = {.head_ = 0, .tail_ = FpsMaxSamples - 1};

void setup_cube_points(void) {
  const float step_size = 2.0f / (g_cube_dimension - 1);
  float y_offset = -1.0f;
  for (int y = 0; y < g_cube_dimension; ++y) {
    float z_offset = -1.0f;
    for (int z = 0; z < g_cube_dimension; ++z) {
      float x_offset = -1.0f;
      for (int x = 0; x < g_cube_dimension; ++x) {
        g_cube_points
          [(y * g_cube_dimension * g_cube_dimension) + (z * g_cube_dimension)
           + x] = (point3f_t){x_offset, y_offset, z_offset};
        x_offset += step_size;
      }
      z_offset += step_size;
    }
    y_offset += step_size;
  }
}

void setup(void) {
  create_color_buffer();
  setup_cube_points();
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

point2f_t project(const point3f_t point, const float fov) {
  return (point2f_t){
    .x = (fov * point.x) / point.z, .y = (fov * point.y) / point.z};
}

double seconds_elapsed(
  const uint64_t old_counter, const uint64_t current_counter) {
  return (current_counter - old_counter)
       / (double)SDL_GetPerformanceFrequency();
}

float maxf(float lhs, float rhs) {
  if (lhs > rhs) {
    return lhs;
  }
  return rhs;
}

void update(void) {
  // reference: https://davidgow.net/handmadepenguin/ch18.html
  // seconds elapsed since last update
  const double seconds =
    seconds_elapsed(g_previous_frame_time, SDL_GetPerformanceCounter());
  if (seconds < seconds_per_frame()) {
    // wait for one ms less than delay (due to precision issues)
    fprintf(stderr, "seconds: %f\n", seconds);
    const double remainder_s = (double)seconds_per_frame() - seconds;
    fprintf(stderr, "remainder_s: %f\n", remainder_s);
    const double remainder_pad_s = remainder_s - 0.004;
    fprintf(stderr, "remainder_pad_s: %f\n", remainder_pad_s);
    const double remainder_ms = remainder_pad_s * 1000.0;
    fprintf(stderr, "remainder_ms: %f\n", remainder_ms);
    const float remainder_ms_clamped = maxf(remainder_ms, 0.0f);
    const uint32_t delay = (uint32_t)remainder_ms_clamped;
    fprintf(stderr, "delay: %u\n", delay);
    SDL_Delay(delay);
    float t =
      seconds_elapsed(g_previous_frame_time, SDL_GetPerformanceCounter());
    fprintf(stderr, "elap: %f\n", t);
    fprintf(stderr, "spf: %f\n", seconds_per_frame());
    // check we didn't wait too long and get behind
    assert(t < seconds_per_frame());
    // busy wait for the remaining time
    while (seconds_elapsed(g_previous_frame_time, SDL_GetPerformanceCounter())
           < seconds_per_frame()) {
    }
  }
  g_previous_frame_time = SDL_GetPerformanceCounter();

  const int64_t time_window =
    calculateWindow(&g_fps, SDL_GetPerformanceCounter());
  const double framerate =
    (double)(FpsMaxSamples - 1)
    / ((double)(time_window) / SDL_GetPerformanceFrequency());

  fprintf(stderr, "fps: %f\n", framerate);

  g_cube_rotation.x += 0.01f;
  g_cube_rotation.y += 0.01f;
  g_cube_rotation.z += 0.01f;

  for (int i = 0; i < g_cube_point_count; ++i) {
    const point3f_t point = g_cube_points[i];
    const point3f_t rotated_point = point3f_rotate_z(
      point3f_rotate_y(
        point3f_rotate_x(point, g_cube_rotation.x), g_cube_rotation.y),
      g_cube_rotation.z);
    const point3f_t transformed_point = point3f_sub_vec3f(
      rotated_point, (vec3f_t){0.0f, 0.0f, g_camera_position.z});
    g_projected_cube_points[i] = project(transformed_point, 1024.0f);
  }
}

void render(void) {
  renderer_clear();

  for (int i = 0; i < g_cube_point_count; ++i) {
    const point2f_t projected_point = g_projected_cube_points[i];
    draw_rect(
      &(rect_t){
        (point2i_t){
          .x = projected_point.x + window_width() / 2,
          .y = projected_point.y + window_height() / 2},
        (size2i_t){.width = 4, .height = 4}},
      0xffffff00);
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
