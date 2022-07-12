#ifndef DISPLAY_H
#define DISPLAY_H

#include <stdbool.h>
#include <stdint.h>

struct SDL_Window;
struct SDL_Renderer;
struct SDL_Texture;

struct point2i_t;
struct point2f_t;
struct point3f_t;
struct rect_t;
struct projected_triangle_t;
struct tex2f_t;
struct texture_t;

int32_t fps(void);
float seconds_per_frame(void);
double seconds_elapsed(uint64_t old_counter, uint64_t current_counter);

bool initialize_window(void);
void deinitialize_window(void);

void create_color_buffer(void);
void destroy_color_buffer(void);

void create_depth_buffer(void);
void destroy_depth_buffer(void);

void draw_pixel(struct point2i_t point, uint32_t color);
void draw_texel(
  struct point2i_t point, struct tex2f_t uv, struct texture_t texture);
void draw_grid(int spacing, uint32_t color);
void draw_rect(struct rect_t rect, uint32_t color);
void draw_line(struct point2i_t p0, struct point2i_t p1, uint32_t color);
void draw_wire_triangle(struct projected_triangle_t triangle, uint32_t color);
void draw_filled_triangle(struct projected_triangle_t triangle, uint32_t color);
void draw_textured_triangle(
  struct projected_triangle_t triangle, struct texture_t texture);

void render_color_buffer(void);
void clear_color_buffer(uint32_t color);
void clear_depth_buffer(void);

void renderer_clear(void);
void renderer_present(void);

int window_width(void);
int window_height(void);

struct point2f_t projectf(struct point3f_t point, float fov);
struct point2i_t projecti(struct point3f_t point, float fov);

#endif // DISPLAY_H
