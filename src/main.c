#include <SDL.h>

#include "array.h"
#include "camera.h"
#include "display.h"
#include "fps.h"
#include "lighting.h"
#include "math-types.h"
#include "mesh.h"
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

projected_triangle_t* g_triangles_to_render = NULL;
camera_t g_camera = {};
uint64_t g_previous_frame_time = 0;
Fps g_fps = {.head_ = 0, .tail_ = FpsMaxSamples - 1};
display_mode_e g_display_mode = display_mode_textured;
bool g_backface_culling = true;
mat44f_t g_perspective_projection;
const vec3f_t g_light_direction = {0.0f, 0.0f, 1.0f};
texture_t g_texture;
int g_projected_count = 0; // triangles projected on a given frame

void setup(void) {
  create_color_buffer();
  create_depth_buffer();
  load_obj_mesh_data("assets/cube.obj");
  // load_cube_mesh_data();
  g_perspective_projection = mat44f_perspective_projection(
    (float)window_width() / (float)window_height(),
    radians_from_degrees(60.0f),
    0.1f,
    100.0f);
  g_texture = load_png_texture_data("assets/redbrick.png");

  g_camera.pivot = (point3f_t){.x = 0.0f, .y = 0.0f, .z = 5.0f};
  g_camera.offset = (vec3f_t){.z = -5.0f};
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
      } else if (event.key.keysym.sym == SDLK_LEFT) {
        g_camera.yaw -= 0.01f;
      } else if (event.key.keysym.sym == SDLK_RIGHT) {
        g_camera.yaw += 0.01f;
      } else if (event.key.keysym.sym == SDLK_UP) {
        g_camera.pitch -= 0.01f;
      } else if (event.key.keysym.sym == SDLK_DOWN) {
        g_camera.pitch += 0.01f;
      } else if (event.key.keysym.sym == SDLK_w) {
        const mat33f_t rotation = camera_rotation(g_camera);
        g_camera.pivot = point3f_add_vec3f(
          g_camera.pivot,
          mat33f_multiply_vec3f(rotation, (vec3f_t){0.0f, 0.0f, 0.1f}));
      } else if (event.key.keysym.sym == SDLK_a) {
        const mat33f_t rotation = camera_rotation(g_camera);
        g_camera.pivot = point3f_add_vec3f(
          g_camera.pivot,
          mat33f_multiply_vec3f(rotation, (vec3f_t){-0.1f, 0.0f, 0.0f}));
      } else if (event.key.keysym.sym == SDLK_s) {
        const mat33f_t rotation = camera_rotation(g_camera);
        g_camera.pivot = point3f_add_vec3f(
          g_camera.pivot,
          mat33f_multiply_vec3f(rotation, (vec3f_t){0.0f, 0.0f, -0.1f}));
      } else if (event.key.keysym.sym == SDLK_d) {
        const mat33f_t rotation = camera_rotation(g_camera);
        g_camera.pivot = point3f_add_vec3f(
          g_camera.pivot,
          mat33f_multiply_vec3f(rotation, (vec3f_t){0.1f, 0.0f, 0.0f}));
      } else if (event.key.keysym.sym == SDLK_q) {
        g_camera.pivot =
          point3f_add_vec3f(g_camera.pivot, (vec3f_t){0.0f, -0.1f, 0.0f});
      } else if (event.key.keysym.sym == SDLK_e) {
        g_camera.pivot =
          point3f_add_vec3f(g_camera.pivot, (vec3f_t){0.0f, 0.1f, 0.0f});
      }
    default:
      break;
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
  g_previous_frame_time = SDL_GetPerformanceCounter();
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

void update(void) {
  wait_to_update();
  calculate_framerate();

  g_model.rotation =
    vec3f_add_vec3f(g_model.rotation, (vec3f_t){0.0f, 0.0f, 0.0f});
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

    point3f_t transformed_vertices[3];
    for (int v = 0; v < 3; ++v) {
      const mat34f_t cam = camera_view(g_camera);
      transformed_vertices[v] = mat34f_multiply_point3f(
        cam, mat34f_multiply_point3f(model_transform, face_vertices[v]));
    }

    const point3f_t a = transformed_vertices[0];
    const point3f_t b = transformed_vertices[1];
    const point3f_t c = transformed_vertices[2];
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

    g_projected_count++;

    projected_triangle_t projected_triangle = {
      .color = apply_light_intensity(
        0xffffff, -vec3f_dot_vec3f(normal, g_light_direction)),
      .vertices = {
        {.uv = g_model.mesh.uvs[mesh_face.uv_indices[0] - 1]},
        {.uv = g_model.mesh.uvs[mesh_face.uv_indices[1] - 1]},
        {.uv = g_model.mesh.uvs[mesh_face.uv_indices[2] - 1]}}};

    for (int v = 0; v < 3; ++v) {
      const point4f_t projected_point = mat44f_project_point3f(
        g_perspective_projection, transformed_vertices[v]);

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

  while (is_running) {
    is_running = process_input();
    update();
    render();
  }

  teardown();

  return 0;
}
