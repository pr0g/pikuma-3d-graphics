#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

typedef struct
{
  int x;
  int y;
  int width;
  int height;
} rect_t;

bool initialize_window(void);
void deinitialize_window(void);

void create_color_buffer(void);
void destroy_color_buffer(void);

void draw_grid(const int spacing, const int32_t color);
void draw_rect(const rect_t* rect, int32_t color);

void render_color_buffer(void);
void clear_color_buffer(const uint32_t color);

void renderer_clear(void);
void renderer_present(void);

#endif // DISPLAY_H
