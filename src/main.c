#include <SDL.h>

#include "array.h"
#include "camera.h"
#include "display.h"
#include "fps.h"
#include "frustum.h"
#include "lighting.h"
#include "math-types.h"
#include "mesh.h"
#include "polygon.h"
#include "texture.h"
#include "upng/upng.h"

#include <assert.h>

typedef enum display_mode_e {
  display_mode_wireframe_vertices,
  display_mode_wireframe,
  display_mode_filled,
  display_mode_filled_wireframe,
  display_mode_textured,
  display_mode_textured_wireframe
} display_mode_e;

typedef enum movement_e {
  movement_up = 1 << 0,
  movement_down = 1 << 1,
  movement_left = 1 << 2,
  movement_right = 1 << 3,
  movement_forward = 1 << 4,
  movement_backward = 1 << 5
} movement_e;

projected_triangle_t* g_triangles_to_render = NULL;
camera_t g_camera = {};
uint64_t g_previous_frame_time = 0;
Fps g_fps = {.head_ = 0, .tail_ = FpsMaxSamples - 1};
display_mode_e g_display_mode = display_mode_wireframe;
bool g_backface_culling = true;
mat44f_t g_perspective_projection;
frustum_planes_t g_frustum_planes;
const vec3f_t g_light_direction = {.z = 1.0f};
texture_t g_texture;
int g_projected_count = 0; // triangles projected on a given frame
point2i_t g_mouse_position = {};
bool g_mouse_down = false;
int8_t g_movement = 0;

void setup(void) {
  create_color_buffer();
  create_depth_buffer();
  load_obj_mesh_data("assets/cube.obj");
  // load_cube_mesh_data();
  const float aspect_ratio = (float)window_width() / (float)window_height();
  const float vertical_fov = radians_from_degrees(60.0f);
  const float near = 0.1f;
  const float far = 100.0f;
  g_perspective_projection =
    mat44f_perspective_projection(aspect_ratio, vertical_fov, near, far);
  g_frustum_planes =
    build_frustum_planes(aspect_ratio, vertical_fov, near, far);
  g_texture = load_png_texture_data("assets/redbrick.png");

  g_camera.pivot = (point3f_t){.x = 0.0f, .y = 0.0f, .z = 0.0f};
  g_camera.offset = (vec3f_t){.z = 0.0f};
}

bool process_input(void) {
  for (SDL_Event event; SDL_PollEvent(&event) != 0;) {
    switch (event.type) {
      case SDL_QUIT:
        return false;
      case SDL_MOUSEMOTION: {
        const SDL_MouseMotionEvent* mouse_motion_event =
          (const SDL_MouseMotionEvent*)&event;
        const point2i_t previous_mouse_position = g_mouse_position;
        g_mouse_position =
          (point2i_t){.x = mouse_motion_event->x, .y = mouse_motion_event->y};
        if (g_mouse_down) {
          const vec2i_t mouse_delta =
            point2i_sub_point2i(g_mouse_position, previous_mouse_position);
          g_camera.pitch += mouse_delta.y * 0.01f;
          g_camera.yaw += mouse_delta.x * 0.01f;
        }
      } break;
      case SDL_MOUSEBUTTONDOWN: {
        g_mouse_down = true;
      } break;
      case SDL_MOUSEBUTTONUP: {
        g_mouse_down = false;
      } break;
      case SDL_KEYDOWN: {
        if (event.key.keysym.sym == SDLK_ESCAPE) {
          return false;
        }
        if (event.key.keysym.sym == SDLK_1) {
          g_display_mode = display_mode_wireframe_vertices;
        } else if (event.key.keysym.sym == SDLK_2) {
          g_display_mode = display_mode_wireframe;
        } else if (event.key.keysym.sym == SDLK_3) {
          g_display_mode = display_mode_filled;
        } else if (event.key.keysym.sym == SDLK_4) {
          g_display_mode = display_mode_filled_wireframe;
        } else if (event.key.keysym.sym == SDLK_5) {
          g_display_mode = display_mode_textured;
        } else if (event.key.keysym.sym == SDLK_6) {
          g_display_mode = display_mode_textured_wireframe;
        } else if (event.key.keysym.sym == SDLK_c) {
          g_backface_culling = !g_backface_culling;
        } else if (event.key.keysym.sym == SDLK_w) {
          g_movement |= movement_forward;
        } else if (event.key.keysym.sym == SDLK_a) {
          g_movement |= movement_left;
        } else if (event.key.keysym.sym == SDLK_s) {
          g_movement |= movement_backward;
        } else if (event.key.keysym.sym == SDLK_d) {
          g_movement |= movement_right;
        } else if (event.key.keysym.sym == SDLK_q) {
          g_movement |= movement_down;
        } else if (event.key.keysym.sym == SDLK_e) {
          g_movement |= movement_up;
        }
      } break;
      case SDL_KEYUP: {
        if (event.key.keysym.sym == SDLK_w) {
          g_movement &= ~movement_forward;
        } else if (event.key.keysym.sym == SDLK_a) {
          g_movement &= ~movement_left;
        } else if (event.key.keysym.sym == SDLK_s) {
          g_movement &= ~movement_backward;
        } else if (event.key.keysym.sym == SDLK_d) {
          g_movement &= ~movement_right;
        } else if (event.key.keysym.sym == SDLK_q) {
          g_movement &= ~movement_down;
        } else if (event.key.keysym.sym == SDLK_e) {
          g_movement &= ~movement_up;
        }
      } break;
      default:
        break;
    }
  }
  return true;
}

void wait_to_update(void) {
  // reference: https://davidgow.net/handmadepenguin/ch18.html
  // seconds elapsed since last update
  const double seconds =
    seconds_elapsed(g_previous_frame_time, SDL_GetPerformanceCounter());
  if (seconds < seconds_per_frame()) {
    // wait for one ms less than delay (due to precision issues)
    const double remainder_s = (double)seconds_per_frame() - seconds;
    // wait 4ms less than actual remainder due to resolution of SDL_Delay
    // (we don't want to delay/sleep too long and get behind)
    const double remainder_pad_s = remainder_s - 0.004;
    const double remainder_pad_ms = remainder_pad_s * 1000.0;
    const double remainder_pad_ms_clamped = fmax(remainder_pad_ms, 0.0);
    const uint32_t delay = (uint32_t)remainder_pad_ms_clamped;
    SDL_Delay(delay);
    const double seconds_left =
      seconds_elapsed(g_previous_frame_time, SDL_GetPerformanceCounter());
    // check we didn't wait too long and get behind
    assert(seconds_left < seconds_per_frame());
    // busy wait for the remaining time
    while (seconds_elapsed(g_previous_frame_time, SDL_GetPerformanceCounter())
           < seconds_per_frame()) {
      ;
    }
  }
}

void calculate_framerate(void) {
  const int64_t time_window =
    calculate_window(&g_fps, SDL_GetPerformanceCounter());
  if (time_window != -1) {
    const double framerate =
      (double)(FpsMaxSamples - 1)
      / ((double)(time_window) / SDL_GetPerformanceFrequency());
    // fprintf(stderr, "fps: %f\n", framerate);
  }
}

static void update_movement(const float delta_time) {
  const float speed = delta_time * 10.0f;
  if ((g_movement & movement_forward) != 0) {
    const mat33f_t rotation = camera_rotation(g_camera);
    g_camera.pivot = point3f_add_vec3f(
      g_camera.pivot, mat33f_multiply_vec3f(rotation, (vec3f_t){.z = speed}));
  } else if ((g_movement & movement_left) != 0) {
    const mat33f_t rotation = camera_rotation(g_camera);
    g_camera.pivot = point3f_add_vec3f(
      g_camera.pivot, mat33f_multiply_vec3f(rotation, (vec3f_t){.x = -speed}));
  } else if ((g_movement & movement_backward) != 0) {
    const mat33f_t rotation = camera_rotation(g_camera);
    g_camera.pivot = point3f_add_vec3f(
      g_camera.pivot, mat33f_multiply_vec3f(rotation, (vec3f_t){.z = -speed}));
  } else if ((g_movement & movement_right) != 0) {
    const mat33f_t rotation = camera_rotation(g_camera);
    g_camera.pivot = point3f_add_vec3f(
      g_camera.pivot, mat33f_multiply_vec3f(rotation, (vec3f_t){.x = speed}));
  } else if ((g_movement & movement_down) != 0) {
    g_camera.pivot = point3f_add_vec3f(g_camera.pivot, (vec3f_t){.y = -speed});
  } else if ((g_movement & movement_up) != 0) {
    g_camera.pivot = point3f_add_vec3f(g_camera.pivot, (vec3f_t){.y = speed});
  }
}

void update(void) {
  const double delta_time =
    seconds_elapsed(g_previous_frame_time, SDL_GetPerformanceCounter());
  g_previous_frame_time = SDL_GetPerformanceCounter();

  wait_to_update();
  calculate_framerate();

  update_movement(delta_time);

  g_model.rotation =
    vec3f_add_vec3f(g_model.rotation, (vec3f_t){0.0f, delta_time * 0.0f, 0.0f});
  // vec3f_add_vec3f(g_model.scale, (vec3f_t){0.002f, 0.0f, 0.0f});
  // g_model.scale = (vec3f_t){0.5f, 0.5f, 0.5f};
  // g_model.translation =
  //   vec3f_add_vec3f(g_model.translation, (vec3f_t){0.01f, 0.0f, 0.0f});

  const mat33f_t scale = mat33f_scale_from_vec3f(g_model.scale);
  const mat34f_t translation =
    mat34f_translation_from_vec3f(g_model.translation);
  const mat33f_t rotation_x = mat33f_x_rotation_from_float(g_model.rotation.x);
  const mat33f_t rotation_y = mat33f_y_rotation_from_float(g_model.rotation.y);
  const mat33f_t rotation_z = mat33f_z_rotation_from_float(g_model.rotation.z);

  const mat33f_t rotation = mat33f_multiply_mat33f(
    rotation_z, mat33f_multiply_mat33f(rotation_y, rotation_x));
  const mat34f_t model_transform = mat34f_multiply_mat33f(
    mat34f_multiply_mat33f(translation, rotation), scale);

  g_projected_count = 0;
  for (int face_index = 0, face_count = array_length(g_model.mesh.faces);
       face_index < face_count;
       ++face_index) {
    const face_t mesh_face = g_model.mesh.faces[face_index];
    const point3f_t face_vertices[] = {
      g_model.mesh.vertices[mesh_face.vert_indices[0] - 1],
      g_model.mesh.vertices[mesh_face.vert_indices[1] - 1],
      g_model.mesh.vertices[mesh_face.vert_indices[2] - 1]};

    const mat34f_t view = camera_view(g_camera);

    triangle_t transformed_triangle;
    for (int v = 0; v < 3; ++v) {
      transformed_triangle.vertices[v] = mat34f_multiply_point3f(
        view, mat34f_multiply_point3f(model_transform, face_vertices[v]));
    }

    const point3f_t a = transformed_triangle.vertices[0];
    const point3f_t b = transformed_triangle.vertices[1];
    const point3f_t c = transformed_triangle.vertices[2];
    const vec3f_t edge_ab = vec3f_normalized(point3f_sub_point3f(b, a));
    const vec3f_t edge_ac = vec3f_normalized(point3f_sub_point3f(c, a));
    const vec3f_t normal =
      vec3f_normalized(vec3f_cross_vec3f(edge_ab, edge_ac));

    if (g_backface_culling) {
      const vec3f_t camera_direction = point3f_sub_point3f((point3f_t){}, a);
      const float view_dot = vec3f_dot_vec3f(normal, camera_direction);
      if (view_dot < 0.0f) {
        continue;
      }
    }

    // clipping
    polygon_t polygon = build_polygon_from_triangle(transformed_triangle);
    clip_polygon_against_frustum(&polygon, g_frustum_planes);

    // triangulate polygon
    triangle_t* clipped_triangles = triangles_from_polygon(polygon);

    const int triangle_count = array_length(clipped_triangles);
    for (int t = 0; t < triangle_count; ++t) {
      g_projected_count++;
      projected_triangle_t projected_triangle = {
        .color = apply_light_intensity(
          0xffffff,
          -vec3f_dot_vec3f(
            normal, mat34f_multiply_vec3f(view, g_light_direction))),
        .vertices = {
          {.uv = g_model.mesh.uvs[mesh_face.uv_indices[0] - 1]},
          {.uv = g_model.mesh.uvs[mesh_face.uv_indices[1] - 1]},
          {.uv = g_model.mesh.uvs[mesh_face.uv_indices[2] - 1]}}};

      for (int v = 0; v < 3; ++v) {
        const point4f_t projected_point = mat44f_project_point3f(
          g_perspective_projection, clipped_triangles[t].vertices[v]);

        const point2f_t projected_point_2d = mat22f_multiply_point2f(
          mat22f_scale_from_floats(
            (float)window_width() / 2.0f, (float)window_height() / -2.0f),
          point2f_from_point4f(projected_point));

        projected_triangle.vertices[v].point = point2i_add_vec2i(
          point2i_from_point2f(projected_point_2d),
          (vec2i_t){window_width() / 2, window_height() / 2});
        projected_triangle.vertices[v].z = projected_point.z;
        projected_triangle.vertices[v].w = projected_point.w;
      }

      if (array_length(g_triangles_to_render) < g_projected_count) {
        array_push(g_triangles_to_render, projected_triangle);
      } else {
        g_triangles_to_render[g_projected_count - 1] = projected_triangle;
      }
    }

    array_free(polygon.vertices);
    array_free(clipped_triangles);
  }
}

void render(void) {
  renderer_clear();

  for (int i = 0, triangle_count = g_projected_count; i < triangle_count; ++i) {
    switch (g_display_mode) {
      case display_mode_filled:
        draw_filled_triangle(
          g_triangles_to_render[i], g_triangles_to_render[i].color);
        break;
      case display_mode_filled_wireframe:
        draw_filled_triangle(
          g_triangles_to_render[i], g_triangles_to_render[i].color);
        draw_wire_triangle(g_triangles_to_render[i], 0xff000000);
        break;
      case display_mode_wireframe_vertices: {
        draw_wire_triangle(g_triangles_to_render[i], 0xff00ffff);
        for (int p = 0; p < 3; ++p) {
          const point2i_t point = g_triangles_to_render[i].vertices[p].point;
          draw_rect(
            (rect_t){
              (point2i_t){.x = point.x - 2, .y = point.y - 2},
              (size2i_t){.width = 5, .height = 5}},
            0xffffffff);
        }
      } break;
      case display_mode_wireframe:
        draw_wire_triangle(g_triangles_to_render[i], 0xff00ffff);
        break;
      case display_mode_textured:
        draw_textured_triangle(g_triangles_to_render[i], g_texture);
        break;
      case display_mode_textured_wireframe:
        draw_textured_triangle(g_triangles_to_render[i], g_texture);
        draw_wire_triangle(g_triangles_to_render[i], 0xffffffff);
        break;
    }
  }

  render_color_buffer();
  clear_color_buffer(0xff000000);
  clear_depth_buffer();

  renderer_present();
}

void teardown(void) {
  array_free(g_triangles_to_render);
  g_triangles_to_render = NULL;
  upng_free(g_texture.png_texture);
  array_free(g_model.mesh.faces);
  array_free(g_model.mesh.vertices);
  destroy_depth_buffer();
  destroy_color_buffer();
  deinitialize_window();
}

int main(int argc, char** argv) {
  bool is_running = initialize_window();

  setup();

  g_previous_frame_time = SDL_GetPerformanceCounter();
  while (is_running) {
    is_running = process_input();
    update();
    render();
  }

  teardown();

  return 0;
}
