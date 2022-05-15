#include "display.h"

#include <SDL.h>

#include <stdio.h>

// global data
struct SDL_Window* g_window = NULL;
struct SDL_Renderer* g_renderer = NULL;
uint32_t* g_color_buffer = NULL;
struct SDL_Texture* g_color_buffer_texture = NULL;

int g_window_width = 800;
int g_window_height = 600;

bool initialize_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  SDL_DisplayMode display_mode;
  SDL_GetCurrentDisplayMode(0, &display_mode);
  g_window_width = display_mode.w;
  g_window_height = display_mode.h;

  g_window = SDL_CreateWindow(
    NULL,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    g_window_width,
    g_window_height,
    SDL_WINDOW_BORDERLESS);

  if (!g_window) {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  g_renderer = SDL_CreateRenderer(g_window, -1, 0);

  if (!g_renderer) {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return false;
  }

  SDL_SetWindowFullscreen(g_window, SDL_WINDOW_FULLSCREEN);

  return true;
}

void draw_grid(const int spacing, const int32_t color) {
  for (int grid_col = 0; grid_col < g_window_width; grid_col += spacing) {
    for (int row = 0; row < g_window_height; ++row) {
      g_color_buffer[row * g_window_width + grid_col] = color;
    }
  }
  for (int col = 0; col < g_window_width; ++col) {
    for (int grid_row = 0; grid_row < g_window_height; grid_row += spacing) {
      g_color_buffer[grid_row * g_window_width + col] = color;
    }
  }
}

void draw_rect(const rect_t* rect, int32_t color) {
  for (int y = rect->y; y < rect->y + rect->height; ++y) {
    for (int x = rect->x; x < rect->x + rect->width; ++x) {
      g_color_buffer[y * g_window_width + x] = color;
    }
  }
}

void clear_color_buffer(const uint32_t color) {
  for (int col = 0; col < g_window_width; ++col) {
    for (int row = 0; row < g_window_height; ++row) {
      g_color_buffer[row * g_window_width + col] = color;
    }
  }
}

void render_color_buffer(void) {
  SDL_UpdateTexture(
    g_color_buffer_texture,
    NULL,
    g_color_buffer,
    g_window_width * sizeof(uint32_t));
  SDL_RenderCopy(g_renderer, g_color_buffer_texture, NULL, NULL);
}

void deinitialize_window(void) {
  SDL_DestroyRenderer(g_renderer);
  SDL_DestroyWindow(g_window);
  SDL_Quit();
}

void create_color_buffer(void) {
  g_color_buffer = malloc(sizeof(uint32_t) * g_window_width * g_window_height);
  g_color_buffer_texture = SDL_CreateTexture(
    g_renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    g_window_width,
    g_window_height);
}

void destroy_color_buffer(void) {
  SDL_DestroyTexture(g_color_buffer_texture);
  free(g_color_buffer);
}

void renderer_clear(void) {
  SDL_SetRenderDrawColor(g_renderer, 255, 0, 0, 255);
  SDL_RenderClear(g_renderer);
}

void renderer_present(void) {
  SDL_RenderPresent(g_renderer);
}
