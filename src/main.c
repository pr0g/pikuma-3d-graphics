#include <SDL.h>

#include "display.h"

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

void update(void) {
}

void render(void) {
  renderer_clear();

  draw_grid(10, 0xff444444);
  draw_rect(
    &(rect_t){.x = 60, .y = 50, .width = 100, .height = 80}, 0xffff0000);
  draw_pixel(&(point2_t){.x = 100, .y = 200}, 0xffffffff);

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
