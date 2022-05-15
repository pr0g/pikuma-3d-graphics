#include "display.h"

#include <SDL.h>

#include <stdio.h>

// global data
static struct SDL_Window* s_window = NULL;
static struct SDL_Renderer* s_renderer = NULL;
static uint32_t* s_color_buffer = NULL;
static struct SDL_Texture* s_color_buffer_texture = NULL;

static int s_window_width = 800;
static int s_window_height = 600;

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

  SDL_SetWindowFullscreen(s_window, SDL_WINDOW_FULLSCREEN);

  return true;
}

void draw_grid(const int spacing, const int32_t color) {
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

void draw_rect(const rect_t* rect, int32_t color) {
  for (int y = rect->y; y < rect->y + rect->height; ++y) {
    for (int x = rect->x; x < rect->x + rect->width; ++x) {
      s_color_buffer[y * s_window_width + x] = color;
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
