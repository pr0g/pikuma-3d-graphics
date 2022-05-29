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

int32_t fps(void);
int32_t frame_target_time(void);
float seconds_per_frame(void);
double seconds_elapsed(uint64_t old_counter, uint64_t current_counter);

bool initialize_window(void);
void deinitialize_window(void);

void create_color_buffer(void);
void destroy_color_buffer(void);

void draw_pixel(struct point2i_t point, uint32_t color);
void draw_grid(int spacing, uint32_t color);
void draw_rect(const struct rect_t* rect, uint32_t color);
void draw_line(struct point2i_t p0, struct point2i_t p1, uint32_t color);

void render_color_buffer(void);
void clear_color_buffer(const uint32_t color);

void renderer_clear(void);
void renderer_present(void);

int window_width(void);
int window_height(void);

struct point2f_t projectf(struct point3f_t point, float fov);
struct point2i_t projecti(struct point3f_t point, float fov);

#endif // DISPLAY_H
