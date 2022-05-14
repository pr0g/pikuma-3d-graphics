#include <SDL.h>

#include <stdbool.h>
#include <stdint.h>
#include <stdio.h>

// global data
SDL_Window* window = NULL;
SDL_Renderer* renderer = NULL;

uint32_t* color_buffer = NULL;
SDL_Texture* color_buffer_texture = NULL;

const int window_width = 800;
const int window_height = 600;

bool initialize_window(void) {
  if (SDL_Init(SDL_INIT_EVERYTHING) != 0) {
    fprintf(stderr, "Error initializing SDL.\n");
    return false;
  }

  window = SDL_CreateWindow(
    NULL,
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    window_width,
    window_height,
    SDL_WINDOW_BORDERLESS);

  if (!window) {
    fprintf(stderr, "Error creating SDL window.\n");
    return false;
  }

  renderer = SDL_CreateRenderer(window, -1, 0);

  if (!renderer) {
    fprintf(stderr, "Error creating SDL renderer.\n");
    return false;
  }

  return true;
}

void setup(void) {
  color_buffer = malloc(sizeof(uint32_t) * window_width * window_height);
  color_buffer_texture = SDL_CreateTexture(
    renderer,
    SDL_PIXELFORMAT_ARGB8888,
    SDL_TEXTUREACCESS_STREAMING,
    window_width,
    window_height);
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

void update(void) {
}

void clear_color_buffer(const uint32_t color) {
  for (int col = 0; col < window_width; ++col) {
    for (int row = 0; row < window_height; ++row) {
      color_buffer[row * window_width + col] = color;
    }
  }
}

void render_color_buffer(void) {
  SDL_UpdateTexture(
    color_buffer_texture, NULL, color_buffer, window_width * sizeof(uint32_t));
  SDL_RenderCopy(renderer, color_buffer_texture, NULL, NULL);
}

void render(void) {
  SDL_SetRenderDrawColor(renderer, 255, 0, 0, 255);
  SDL_RenderClear(renderer);

  render_color_buffer();
  clear_color_buffer(0xffffff00);

  SDL_RenderPresent(renderer);
}

void teardown(void) {
  free(color_buffer);
  SDL_DestroyRenderer(renderer);
  SDL_DestroyWindow(window);
  SDL_Quit();
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
