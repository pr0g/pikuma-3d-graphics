#include "display.h"

#include "math-types.h"
#include "triangle.h"

#include <SDL.h>

#include <stdio.h>

// global data
static struct SDL_Window* s_window = NULL;
static struct SDL_Renderer* s_renderer = NULL;
static uint32_t* s_color_buffer = NULL;
static struct SDL_Texture* s_color_buffer_texture = NULL;

static int s_window_width = 800;
static int s_window_height = 600;

int32_t fps(void) {
  return 30;
}

int32_t frame_target_time(void) {
  return 1000 / fps();
}

float seconds_per_frame(void) {
  return 1.0f / (float)fps();
}

double seconds_elapsed(
  const uint64_t old_counter, const uint64_t current_counter) {
  return (current_counter - old_counter)
       / (double)SDL_GetPerformanceFrequency();
}

void swapf(float* lhs, float* rhs) {
  float temp = *lhs;
  *lhs = *rhs;
  *rhs = temp;
}

static void swapi(int* lhs, int* rhs) {
  int temp = *lhs;
  *lhs = *rhs;
  *rhs = temp;
}

bool initialize_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  s_window_width = display_mode.w;
  s_window_height = display_mode.h;

  s_window = SDL_CreateWindow(
    NULL,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    s_window_width,
    s_window_height,
    0 /*SDL_WINDOW_BORDERLESS*/);

  if (!s_window) {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  s_renderer = SDL_CreateRenderer(s_window, -1, 0);

  if (!s_renderer) {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return false;
  }

  // disable fullscreen
  // SDL_SetWindowFullscreen(s_window, SDL_WINDOW_FULLSCREEN);

  return true;
}

void draw_pixel(const point2i_t point, const uint32_t color) {
  if (
    point.x < 0 || point.x >= s_window_width || point.y <= 0
    || point.y >= s_window_height) {
    return;
  }
  s_color_buffer[point.y * s_window_width + point.x] = color;
}

void draw_grid(const int spacing, const uint32_t color) {
  for (int grid_col = 0; grid_col < s_window_width; grid_col += spacing) {
    for (int row = 0; row < s_window_height; ++row) {
      s_color_buffer[row * s_window_width + grid_col] = color;
    }
  }
  for (int col = 0; col < s_window_width; ++col) {
    for (int grid_row = 0; grid_row < s_window_height; grid_row += spacing) {
      s_color_buffer[grid_row * s_window_width + col] = color;
    }
  }
}

void draw_rect(const rect_t rect, const uint32_t color) {
  for (int y = rect.pos.y; y < rect.pos.y + rect.size.height; ++y) {
    for (int x = rect.pos.x; x < rect.pos.x + rect.size.width; ++x) {
      draw_pixel((point2i_t){x, y}, color);
    }
  }
}

void draw_line(const point2i_t p0, const point2i_t p1, const uint32_t color) {
  const vec2i_t delta = point2i_sub_point2i(p1, p0);
  const int side_length = maxi(abs(delta.x), abs(delta.y));
  const vec2f_t inc = vec2i_div_scalar(delta, (float)side_length);
  point2f_t current = point2f_from_point2i(p0);
  for (int i = 0; i <= side_length; ++i) {
    draw_pixel(point2i_from_point2f(current), color);
    current = point2f_add_vec2f(current, inc);
  }
}

void draw_wire_triangle(
  const projected_triangle_t triangle, const uint32_t color) {
  for (int v = 0; v < 3; ++v) {
    draw_line(triangle.points[v], triangle.points[((v + 1) % 3)], color);
  }
}

static int compare_point2f(const void* lhs, const void* rhs) {
  const point2f_t arg1 = *(const point2f_t*)(lhs);
  const point2f_t arg2 = *(const point2f_t*)(rhs);
  if (arg1.y < arg2.y) {
    return -1;
  }
  if (arg1.y > arg2.y) {
    return 1;
  }
  return 0;
}

static void fill_triangle(
  const point2i_t start,
  const point2i_t left,
  const point2i_t right,
  const float direction,
  const uint32_t color) {
  const float inv_slope1 =
    ((float)(left.x - start.x) / (float)(left.y - start.y)) * direction;
  const float inv_slope2 =
    ((float)(right.x - start.x) / (float)(right.y - start.y)) * direction;
  float x_start = (float)start.x;
  float x_end = (float)start.x;
  const int delta = abs(left.y - start.y);
  for (int y = 0; y <= delta; y++) {
    const float current_y = (float)start.y + (float)y * direction;
    draw_line(
      point2i_from_point2f((point2f_t){x_start, current_y}),
      point2i_from_point2f((point2f_t){x_end, current_y}),
      color);
    x_start += inv_slope1;
    x_end += inv_slope2;
  }
}

static void fill_flat_bottom_triangle(
  const projected_triangle_t triangle, const uint32_t color) {
  fill_triangle(
    triangle.points[0], triangle.points[1], triangle.points[2], 1.0f, color);
}

static void fill_flat_top_triangle(
  const projected_triangle_t triangle, const uint32_t color) {
  fill_triangle(
    triangle.points[2], triangle.points[0], triangle.points[1], -1.0f, color);
}

void draw_filled_triangle(projected_triangle_t triangle, const uint32_t color) {
  qsort(
    triangle.points,
    sizeof triangle.points / sizeof *triangle.points,
    sizeof(point2f_t),
    compare_point2f);
  if (triangle.points[1].y == triangle.points[2].y) {
    fill_flat_bottom_triangle(triangle, color);
  } else if (triangle.points[0].y == triangle.points[1].y) {
    fill_flat_top_triangle(triangle, color);
  } else {
    const int my = triangle.points[1].y;
    const int mx = (int)((float)((triangle.points[2].x - triangle.points[0].x)
                   * (triangle.points[1].y - triangle.points[0].y))
                  / (float)(triangle.points[2].y - triangle.points[0].y))
               + triangle.points[0].x;
    fill_flat_bottom_triangle(
      (projected_triangle_t){
        .points = {triangle.points[0], triangle.points[1], {.x = mx, .y = my}}},
      color);
    fill_flat_top_triangle(
      (projected_triangle_t){
        .points = {triangle.points[1], {.x = mx, .y = my}, triangle.points[2]}},
      color);
  }
}

static void texture_triangle(
  const point2i_t start,
  const point2i_t left,
  const point2i_t right,
  const float direction,
  const uint32_t color) {
  const float inv_slope1 =
    ((float)(left.x - start.x) / (float)(left.y - start.y)) * direction;
  const float inv_slope2 =
    ((float)(right.x - start.x) / (float)(right.y - start.y)) * direction;
  float x_start = (float)start.x;
  float x_end = (float)start.x;
  const int delta_y = abs(left.y - start.y);
  for (int y = 0; y <= delta_y; y++) {
    const float current_y = (float)start.y + (float)y * direction;
    const int delta_x = (int)(roundf(x_end) - roundf(x_start));
    const int side_length = abs(delta_x);
    if (side_length != 0) {
      const float inc = (float)delta_x / (float)side_length;
      float current_x = x_start;
      for (int x = 0; x <= side_length; ++x) {
        draw_pixel(point2i_from_point2f((point2f_t){current_x, current_y}), color);
        current_x += inc;
      }
    }
    x_start += inv_slope1;
    x_end += inv_slope2;
  }
}

static void texture_flat_bottom_triangle(
  const projected_triangle_t triangle, const uint32_t color) {
  texture_triangle(
    triangle.points[0], triangle.points[1], triangle.points[2], 1.0f, color);
}

static void texture_flat_top_triangle(
  const projected_triangle_t triangle, const uint32_t color) {
  texture_triangle(
    triangle.points[2], triangle.points[0], triangle.points[1], -1.0f, color);
}

void draw_textured_triangle(
  projected_triangle_t triangle, const uint32_t* texture) {
  qsort(
    triangle.points,
    sizeof triangle.points / sizeof *triangle.points,
    sizeof(point2f_t),
    compare_point2f);
  const int my = triangle.points[1].y;
  const int mx = (int)((float)((triangle.points[2].x - triangle.points[0].x)
                   * (triangle.points[1].y - triangle.points[0].y))
                  / (float)(triangle.points[2].y - triangle.points[0].y))
               + triangle.points[0].x;
  texture_flat_bottom_triangle(
    (projected_triangle_t){
      .points = {triangle.points[0], triangle.points[1], {.x = mx, .y = my}}},
    0xffff00ff);
  texture_flat_top_triangle(
    (projected_triangle_t){
      .points = {triangle.points[1], {.x = mx, .y = my}, triangle.points[2]}},
    0xffff00ff);
}

void clear_color_buffer(const uint32_t color) {
  for (int col = 0; col < s_window_width; ++col) {
    for (int row = 0; row < s_window_height; ++row) {
      s_color_buffer[row * s_window_width + col] = color;
    }
  }
}

void render_color_buffer(void) {
  SDL_UpdateTexture(
    s_color_buffer_texture,
    NULL,
    s_color_buffer,
    s_window_width * sizeof(uint32_t));
  SDL_RenderCopy(s_renderer, s_color_buffer_texture, NULL, NULL);
}

void deinitialize_window(void) {
  SDL_DestroyRenderer(s_renderer);
  SDL_DestroyWindow(s_window);
  SDL_Quit();
}

void create_color_buffer(void) {
  s_color_buffer = malloc(sizeof(uint32_t) * s_window_width * s_window_height);
  s_color_buffer_texture = SDL_CreateTexture(
    s_renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    s_window_width,
    s_window_height);
}

void destroy_color_buffer(void) {
  SDL_DestroyTexture(s_color_buffer_texture);
  free(s_color_buffer);
}

void renderer_clear(void) {
  SDL_SetRenderDrawColor(s_renderer, 255, 0, 0, 255);
  SDL_RenderClear(s_renderer);
}

void renderer_present(void) {
  SDL_RenderPresent(s_renderer);
}

int window_width(void) {
  return s_window_width;
}

int window_height(void) {
  return s_window_height;
}
