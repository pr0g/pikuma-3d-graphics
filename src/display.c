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
    SDL_WINDOW_BORDERLESS);

  if (!s_window) {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  s_renderer = SDL_CreateRenderer(s_window, -1, 0);

  if (!s_renderer) {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return false;
  }

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

void draw_texel(
  const point2i_t point, const tex2f_t uv, const uint32_t* texture) {
  const tex2f_t clamped_uv =
    (tex2f_t){clampf(uv.u, 0.0f, 1.0f), clampf(uv.v, 0.0f, 1.0f)};
  const point2i_t texture_coordinate = point2i_at_proportion_of_size2i(
    (size2i_t){
      .width = redbrick_texture_width() - 1,
      .height = redbrick_texture_height() - 1},
    clamped_uv);
  draw_pixel(
    point,
    texture
      [(redbrick_texture_height() - 1 - texture_coordinate.y) * redbrick_texture_width() + texture_coordinate.x]);
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
    draw_line(
      triangle.vertices[v].point,
      triangle.vertices[((v + 1) % 3)].point,
      color);
  }
}

static int compare_projected_vertex(const void* lhs, const void* rhs) {
  const projected_vertex_t arg1 = *(const projected_vertex_t*)(lhs);
  const projected_vertex_t arg2 = *(const projected_vertex_t*)(rhs);
  if (arg1.point.y < arg2.point.y) {
    return -1;
  }
  if (arg1.point.y > arg2.point.y) {
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
    triangle.vertices[0].point,
    triangle.vertices[1].point,
    triangle.vertices[2].point,
    1.0f,
    color);
}

static void fill_flat_top_triangle(
  const projected_triangle_t triangle, const uint32_t color) {
  fill_triangle(
    triangle.vertices[2].point,
    triangle.vertices[0].point,
    triangle.vertices[1].point,
    -1.0f,
    color);
}

void draw_filled_triangle(projected_triangle_t triangle, const uint32_t color) {
  qsort(
    triangle.vertices,
    sizeof triangle.vertices / sizeof *triangle.vertices,
    sizeof(projected_vertex_t),
    compare_projected_vertex);
  if (triangle.vertices[1].point.y == triangle.vertices[2].point.y) {
    fill_flat_bottom_triangle(triangle, color);
  } else if (triangle.vertices[0].point.y == triangle.vertices[1].point.y) {
    fill_flat_top_triangle(triangle, color);
  } else {
    const int mid_y = triangle.vertices[1].point.y;
    const int mid_x = (int)((float)((triangle.vertices[2].point.x - triangle.vertices[0].point.x)
                   * (triangle.vertices[1].point.y - triangle.vertices[0].point.y))
                  / (float)(triangle.vertices[2].point.y - triangle.vertices[0].point.y))
               + triangle.vertices[0].point.x;
    fill_flat_bottom_triangle(
      (projected_triangle_t){
        .vertices =
          {{.point = triangle.vertices[0].point},
           {.point = triangle.vertices[1].point},
           {.point = (point2i_t){.x = mid_x, .y = mid_y}}}},
      color);
    fill_flat_top_triangle(
      (projected_triangle_t){
        .vertices =
          {{.point = triangle.vertices[1].point},
           {.point = (point2i_t){.x = mid_x, .y = mid_y}},
           {.point = triangle.vertices[2].point}}},
      color);
  }
}

void draw_textured_triangle(
  projected_triangle_t triangle, const uint32_t* texture) {
  qsort(
    triangle.vertices,
    sizeof triangle.vertices / sizeof *triangle.vertices,
    sizeof(projected_vertex_t),
    compare_projected_vertex);

  float inv_slope_1 = 0.0f;
  float inv_slope_2 = 0.0f;
  if (triangle.vertices[1].point.y - triangle.vertices[0].point.y != 0) {
    inv_slope_1 =
      (float)(triangle.vertices[1].point.x - triangle.vertices[0].point.x)
      / (float)abs(triangle.vertices[1].point.y - triangle.vertices[0].point.y);
  }
  if (triangle.vertices[2].point.y - triangle.vertices[0].point.y != 0) {
    inv_slope_2 =
      (float)(triangle.vertices[2].point.x - triangle.vertices[0].point.x)
      / (float)abs(triangle.vertices[2].point.y - triangle.vertices[0].point.y);
  }

  if (triangle.vertices[1].point.y - triangle.vertices[0].point.y != 0) {
    for (int y = triangle.vertices[0].point.y;
         y <= triangle.vertices[1].point.y;
         y++) {
      int x_start =
        triangle.vertices[1].point.x
        + (int)((float)(y - triangle.vertices[1].point.y) * inv_slope_1);
      int x_end =
        triangle.vertices[0].point.x
        + (int)((float)(y - triangle.vertices[0].point.y) * inv_slope_2);

      if (x_end < x_start) {
        swapi(&x_start, &x_end); // swap if x_start is to the right of x_end
      }

      for (int x = x_start; x <= x_end; x++) {
        const point2i_t point = (point2i_t){x, y};
        const barycentric_coords_t barycentric_coords =
          calculate_barycentric_coordinates(
            triangle.vertices[0].point,
            triangle.vertices[1].point,
            triangle.vertices[2].point,
            point);
        const tex2f_t uv = calculate_uv(
          barycentric_coords,
          triangle.vertices[0].uv,
          triangle.vertices[1].uv,
          triangle.vertices[2].uv);
        draw_texel(point, uv, texture);
      }
    }
  }

  inv_slope_1 = 0.0f;
  inv_slope_2 = 0.0f;

  if (triangle.vertices[2].point.y - triangle.vertices[1].point.y != 0) {
    inv_slope_1 =
      (float)(triangle.vertices[2].point.x - triangle.vertices[1].point.x)
      / (float)abs(triangle.vertices[2].point.y - triangle.vertices[1].point.y);
  }
  if (triangle.vertices[2].point.y - triangle.vertices[0].point.y != 0) {
    inv_slope_2 =
      (float)(triangle.vertices[2].point.x - triangle.vertices[0].point.x)
      / (float)abs(triangle.vertices[2].point.y - triangle.vertices[0].point.y);
  }

  if (triangle.vertices[2].point.y - triangle.vertices[1].point.y != 0) {
    for (int y = triangle.vertices[1].point.y;
         y <= triangle.vertices[2].point.y;
         y++) {
      int x_start =
        triangle.vertices[1].point.x
        + (int)((float)(y - triangle.vertices[1].point.y) * inv_slope_1);
      int x_end =
        triangle.vertices[0].point.x
        + (int)((float)(y - triangle.vertices[0].point.y) * inv_slope_2);

      if (x_end < x_start) {
        swapi(&x_start, &x_end);
      }

      for (int x = x_start; x <= x_end; x++) {
        const point2i_t point = (point2i_t){x, y};
        const barycentric_coords_t barycentric_coords =
          calculate_barycentric_coordinates(
            triangle.vertices[0].point,
            triangle.vertices[1].point,
            triangle.vertices[2].point,
            point);
        const tex2f_t uv = calculate_uv(
          barycentric_coords,
          triangle.vertices[0].uv,
          triangle.vertices[1].uv,
          triangle.vertices[2].uv);
        draw_texel(point, uv, texture);
      }
    }
  }
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
