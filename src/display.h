#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

struct point2i_t;
struct rect_t;

bool initialize_window(void);
void deinitialize_window(void);

void create_color_buffer(void);
void destroy_color_buffer(void);

void draw_pixel(const struct point2i_t* point, int32_t color);
void draw_grid(int spacing, int32_t color);
void draw_rect(const struct rect_t* rect, int32_t color);

void render_color_buffer(void);
void clear_color_buffer(const uint32_t color);

void renderer_clear(void);
void renderer_present(void);

#endif // DISPLAY_H
