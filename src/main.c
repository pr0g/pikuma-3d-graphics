#include <SDL.h>

#include "display.h"
#include "math-types.h"

const int g_cube_dimension = 10;
const int g_cube_point_count =
  g_cube_dimension * g_cube_dimension * g_cube_dimension;
point3f_t g_cube_points[g_cube_point_count];
point2f_t g_projected_cube_points[g_cube_point_count];

void setup_cube_points(void) {
  const float step_size = 2.0f / g_cube_dimension;
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

point2f_t project(point3f_t point, const float fov) {
  point2f_t projected_point = {.x = fov * point.x, .y = fov * point.y};
  return projected_point;
}

void update(void) {
  for (int i = 0; i < g_cube_point_count; ++i) {
    const point3f_t point = g_cube_points[i];
    g_projected_cube_points[i] = project(point, 128.0f);
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
