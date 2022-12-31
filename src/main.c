#include "array.h"
#include "camera.h"
#include "display.h"
#include "fps.h"
#include "frustum.h"
#include "lighting.h"
#include "mesh.h"
#include "polygon.h"
#include "texture.h"

#include <as-ops.h>
#include <upng.h>

#include <SDL.h>

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

typedef struct projected_model_t {
  projected_triangle_t* projected_triangles;
  int projected_count;
} projected_model_t;

camera_t g_camera = {0};
uint64_t g_previous_frame_time = 0;
Fps g_fps = {.head_ = 0, .tail_ = FpsMaxSamples - 1};
display_mode_e g_display_mode = display_mode_textured;
bool g_backface_culling = true;
as_mat44f g_perspective_projection;
frustum_planes_t g_frustum_planes;
const as_vec3f g_light_direction = {.z = -0.5f, .y = -0.5f};
as_point2i g_mouse_position = {0};
bool g_mouse_down = false;
int8_t g_movement = 0;
model_t* g_models = NULL;
projected_model_t* g_projected_models = NULL;
int g_projected_model_count = 0;

void setup(void) {
  create_color_buffer();
  create_depth_buffer();
  const float aspect_ratio = (float)window_width() / (float)window_height();
  const float vertical_fov = as_radians_from_degrees(60.0f);
  const float near = 0.1f;
  const float far = 100.0f;
  g_perspective_projection =
    as_mat44f_perspective_projection_depth_zero_to_one_lh(
      aspect_ratio, vertical_fov, near, far);
  g_frustum_planes =
    build_frustum_planes(aspect_ratio, vertical_fov, near, far);

  {
    model_t model =
      load_obj_mesh_with_png_texture("assets/efa.obj", "assets/efa.png");
    model.translation = (as_vec3f){.x = 2.0f, .y = 0.2f, .z = -18.0f};
    model.rotation = (as_vec3f){.y = -as_k_pi * 0.5f};
    array_push(g_models, model);
  }

  {
    model_t model =
      load_obj_mesh_with_png_texture("assets/f22.obj", "assets/f22.png");
    model.translation = (as_vec3f){.x = -2.0f, .y = 0.2f, .z = -18.0f};
    model.rotation = (as_vec3f){.y = -as_k_pi * 0.5f};
    array_push(g_models, model);
  }

  {
    model_t model =
      load_obj_mesh_with_png_texture("assets/f117.obj", "assets/f117.png");
    model.translation = (as_vec3f){.y = 0.2f, .z = -14.0f};
    model.rotation = (as_vec3f){.y = -as_k_pi * 0.5f};
    array_push(g_models, model);
  }

  {
    model_t model =
      load_obj_mesh_with_png_texture("assets/runway.obj", "assets/runway.png");
    array_push(g_models, model);
  }

  g_camera.pitch = as_radians_from_degrees(20.0f);
  g_camera.yaw = as_radians_from_degrees(160.0f);
  g_camera.pivot = (as_point3f){.x = -2.0f, .y = 2.5f, .z = -10.0f};
  g_camera.offset = (as_vec3f){.z = 0.0f};
}

bool process_input(void) {
  for (SDL_Event event; SDL_PollEvent(&event) != 0;) {
    switch (event.type) {
      case SDL_QUIT:
        return false;
      case SDL_MOUSEMOTION: {
        const SDL_MouseMotionEvent* mouse_motion_event =
          (const SDL_MouseMotionEvent*)&event;
        const as_point2i previous_mouse_position = g_mouse_position;
        g_mouse_position =
          (as_point2i){.x = mouse_motion_event->x, .y = mouse_motion_event->y};
        if (g_mouse_down) {
          const as_vec2i mouse_delta =
            as_point2i_sub_point2i(g_mouse_position, previous_mouse_position);
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
    const as_mat33f rotation = camera_rotation(&g_camera);
    g_camera.pivot = as_point3f_add_vec3f(
      g_camera.pivot, as_mat33f_mul_vec3f(&rotation, (as_vec3f){.z = speed}));
  }
  if ((g_movement & movement_left) != 0) {
    const as_mat33f rotation = camera_rotation(&g_camera);
    g_camera.pivot = as_point3f_add_vec3f(
      g_camera.pivot, as_mat33f_mul_vec3f(&rotation, (as_vec3f){.x = -speed}));
  }
  if ((g_movement & movement_backward) != 0) {
    const as_mat33f rotation = camera_rotation(&g_camera);
    g_camera.pivot = as_point3f_add_vec3f(
      g_camera.pivot, as_mat33f_mul_vec3f(&rotation, (as_vec3f){.z = -speed}));
  }
  if ((g_movement & movement_right) != 0) {
    const as_mat33f rotation = camera_rotation(&g_camera);
    g_camera.pivot = as_point3f_add_vec3f(
      g_camera.pivot, as_mat33f_mul_vec3f(&rotation, (as_vec3f){.x = speed}));
  }
  if ((g_movement & movement_down) != 0) {
    g_camera.pivot =
      as_point3f_add_vec3f(g_camera.pivot, (as_vec3f){.y = -speed});
  }
  if ((g_movement & movement_up) != 0) {
    g_camera.pivot =
      as_point3f_add_vec3f(g_camera.pivot, (as_vec3f){.y = speed});
  }
}

void process_graphics_pipeline(model_t* model, const as_mat34f view) {
  g_projected_model_count++;
  if (array_length(g_projected_models) < g_projected_model_count) {
    array_push(g_projected_models, (projected_model_t){0});
  }

  const as_mat33f scale = as_mat33f_scale_from_vec3f(model->scale);
  const as_mat34f translation =
    as_mat34f_translation_from_vec3f(model->translation);
  const as_mat33f rotation_x = as_mat33f_x_axis_rotation(model->rotation.x);
  const as_mat33f rotation_y = as_mat33f_y_axis_rotation(model->rotation.y);
  const as_mat33f rotation_z = as_mat33f_z_axis_rotation(model->rotation.z);

  const as_mat33f rotation_yx = as_mat33f_mul_mat33f(&rotation_y, &rotation_x);
  const as_mat33f rotation = as_mat33f_mul_mat33f(&rotation_z, &rotation_yx);
  const as_mat34f translation_rotation =
    as_mat34f_mul_mat33f(&translation, &rotation);
  const as_mat34f model_transform =
    as_mat34f_mul_mat33f(&translation_rotation, &scale);

  projected_model_t* projected_model =
    &g_projected_models[g_projected_model_count - 1];
  projected_model->projected_count = 0;

  for (int face_index = 0, face_count = array_length(model->mesh.faces);
       face_index < face_count;
       ++face_index) {
    const face_t mesh_face = model->mesh.faces[face_index];
    const as_point3f face_vertices[] = {
      model->mesh.vertices[mesh_face.vert_indices[0] - 1],
      model->mesh.vertices[mesh_face.vert_indices[1] - 1],
      model->mesh.vertices[mesh_face.vert_indices[2] - 1]};

    // model -> view transform
    uv_triangle_t transformed_triangle;
    for (int v = 0; v < 3; ++v) {
      const as_point3f world_position =
        as_mat34f_mul_point3f(&model_transform, face_vertices[v]);
      transformed_triangle.triangle.vertices[v] =
        as_mat34f_mul_point3f(&view, world_position);
      transformed_triangle.uvs[v] =
        model->mesh.uvs[mesh_face.uv_indices[v] - 1];
    }

    // backface culling
    const as_vec3f normal =
      calculate_triangle_normal(transformed_triangle.triangle);
    if (g_backface_culling) {
      const as_vec3f camera_direction = as_point3f_sub_point3f(
        (as_point3f){0}, transformed_triangle.triangle.vertices[0]);
      const float view_dot = as_vec3f_dot_vec3f(normal, camera_direction);
      if (view_dot < 0.0f) {
        continue;
      }
    }

    // clipping
    polygon_t polygon = build_polygon_from_uv_triangle(transformed_triangle);
    clip_polygon_against_frustum(&polygon, g_frustum_planes);

    // triangulate polygon
    uv_triangle_t* clipped_triangles = uv_triangles_from_polygon(polygon);

    const int triangle_count = array_length(clipped_triangles);
    for (int t = 0; t < triangle_count; ++t) {
      projected_model->projected_count++;
      projected_triangle_t projected_triangle = {
        .color = apply_light_intensity(
          0xffffff,
          -as_vec3f_dot_vec3f(
            normal, as_mat34f_mul_vec3f(&view, g_light_direction))),
        .vertices = {
          {.uv = clipped_triangles[t].uvs[0]},
          {.uv = clipped_triangles[t].uvs[1]},
          {.uv = clipped_triangles[t].uvs[2]}}};

      for (int v = 0; v < 3; ++v) {
        // projection and perspective divide
        const as_point4f projected_point = as_mat44f_project_point3f(
          &g_perspective_projection, clipped_triangles[t].triangle.vertices[v]);

        const as_mat22f window_scale = as_mat22f_scale_from_floats(
          (float)window_width() / 2.0f, (float)window_height() / -2.0f);
        const as_point2f projected_point_2d = as_mat22f_mul_point2f(
          &window_scale, as_point2f_from_point4f(projected_point));

        // convert to screen space
        projected_triangle.vertices[v].point = as_point2i_add_vec2i(
          as_point2i_from_point2f(projected_point_2d),
          (as_vec2i){window_width() / 2, window_height() / 2});
        projected_triangle.vertices[v].z = projected_point.z;
        projected_triangle.vertices[v].w = projected_point.w;
      }

      if (
        array_length(projected_model->projected_triangles)
        < projected_model->projected_count) {
        array_push(projected_model->projected_triangles, projected_triangle);
      } else {
        projected_model
          ->projected_triangles[projected_model->projected_count - 1] =
          projected_triangle;
      }
    }
    array_free(polygon.uvs);
    array_free(polygon.vertices);
    array_free(clipped_triangles);
  }
}

void update(void) {
  wait_to_update();

  const int64_t current_counter = SDL_GetPerformanceCounter();
  const double delta_time =
    seconds_elapsed(g_previous_frame_time, current_counter);
  g_previous_frame_time = current_counter;

  calculate_framerate();

  update_movement(delta_time);

  const as_mat34f view = camera_view(&g_camera);
  g_projected_model_count = 0;
  const int model_count = array_length(g_models);
  for (int m = 0; m < model_count; m++) {
    process_graphics_pipeline(&g_models[m], view);
  }
}

void render(void) {
  clear_color_buffer(0xff000000);
  clear_depth_buffer();

  for (int m = 0; m < g_projected_model_count; m++) {
    const model_t* model = &g_models[m];
    const projected_model_t* projected_model = &g_projected_models[m];
    for (int i = 0, triangle_count = projected_model->projected_count;
         i < triangle_count;
         ++i) {
      switch (g_display_mode) {
        case display_mode_filled:
          draw_filled_triangle(
            projected_model->projected_triangles[i],
            projected_model->projected_triangles[i].color);
          break;
        case display_mode_filled_wireframe:
          draw_filled_triangle(
            projected_model->projected_triangles[i],
            projected_model->projected_triangles[i].color);
          draw_wire_triangle(
            projected_model->projected_triangles[i], 0xff000000);
          break;
        case display_mode_wireframe_vertices: {
          draw_wire_triangle(
            projected_model->projected_triangles[i], 0xff00ffff);
          for (int p = 0; p < 3; ++p) {
            const as_point2i point =
              projected_model->projected_triangles[i].vertices[p].point;
            draw_rect(
              (as_rect){
                (as_point2i){.x = point.x - 2, .y = point.y - 2},
                (as_size2i){.width = 5, .height = 5}},
              0xffffffff);
          }
        } break;
        case display_mode_wireframe:
          draw_wire_triangle(
            projected_model->projected_triangles[i], 0xff00ffff);
          break;
        case display_mode_textured:
          draw_textured_triangle(
            projected_model->projected_triangles[i], model->texture);
          break;
        case display_mode_textured_wireframe:
          draw_textured_triangle(
            projected_model->projected_triangles[i], model->texture);
          draw_wire_triangle(
            projected_model->projected_triangles[i], 0xffffffff);
          break;
      }
    }
  }

  render_color_buffer();
  renderer_present();
}

void teardown(void) {
  const int projected_model_count = array_length(g_projected_models);
  for (int m = 0; m < projected_model_count; m++) {
    projected_model_t* projected_model = &g_projected_models[m];
    array_free(projected_model->projected_triangles);
  }
  array_free(g_projected_models);
  const int model_count = array_length(g_models);
  for (int m = 0; m < model_count; ++m) {
    model_t* model = &g_models[m];
    upng_free(model->texture.png_texture);
    array_free(model->mesh.faces);
    array_free(model->mesh.vertices);
    array_free(model->mesh.uvs);
  }
  array_free(g_models);
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
