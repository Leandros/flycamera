#include "../../flycamera.h"

#include <stdio.h>
#include <glad/glad.h>
#include <SDL2/SDL.h>

#include "torus.h"
#include "shader.h"
#include "../glad/src/glad.c"

float GetAspectRatio(SDL_Window * sdl_window);
void CreatePerspectiveProjection(float * proj, float aspect, float fov_y_rad, float n, float f);

int main() {
  SDL_Init(SDL_INIT_VIDEO);

  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
  
  SDL_Window * sdl_window = SDL_CreateWindow(
    "FlyCamera: SDL",
    SDL_WINDOWPOS_CENTERED,
    SDL_WINDOWPOS_CENTERED,
    1280, 720,
    SDL_WINDOW_OPENGL
  );

  SDL_GLContext sdl_glcontext = SDL_GL_CreateContext(sdl_window);
  SDL_GL_MakeCurrent(sdl_window, sdl_glcontext);

  if (!gladLoadGL()) {
    fprintf(stderr, "GLAD can't load OpenGL\n");
    return 1;
  }
  
  GLuint vao = 0;
  GLuint mesh_buffer = 0;
  GLuint mesh_pipeline = 0;

  glGenVertexArrays(1, &vao);
  glGenBuffers(1, &mesh_buffer);

  GLuint program = CreateProgram("vs.vert", "fs.frag");

  glBindBuffer(GL_ARRAY_BUFFER, mesh_buffer);
  glBufferData(GL_ARRAY_BUFFER, sizeof(mesh), mesh, GL_STATIC_DRAW);
  glBindBuffer(GL_ARRAY_BUFFER, 0);

  glBindVertexArray(vao);
  glEnableVertexAttribArray(0);

  glClearColor(168 / 255.f, 204 / 255.f, 207 / 255.f, 1.f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  float cam_vec3[3] = {0.f};
  float cam_quat[4] = {0.f, 0.f, 0.f, 1.f};
  float cam_m3x3[9] = {0.f};
  float cam_proj[4] = {0.f};

  CreatePerspectiveProjection(
    cam_proj,
    GetAspectRatio(sdl_window),
    85.0f * 0.017453f,
    0.01f, 1000.f
  );

  SDL_SetRelativeMouseMode(1);

  Uint32 t_prev = SDL_GetTicks();

  while (1) {
    Uint32 t_curr = SDL_GetTicks();
    double dt = ((t_curr - t_prev) * 60.0) / 1000.0;

    SDL_PumpEvents();
    int mouse_x_rel = 0;
    int mouse_y_rel = 0;
    SDL_GetRelativeMouseState(&mouse_x_rel, &mouse_y_rel);
    const Uint8 * key = SDL_GetKeyboardState(NULL);

    flycamera(
      cam_vec3,
      cam_quat,
      cam_m3x3,
      0.05f,
      0.05f * dt,
      -mouse_x_rel,
      -mouse_y_rel,
      key[SDL_SCANCODE_W],
      key[SDL_SCANCODE_A],
      key[SDL_SCANCODE_S],
      key[SDL_SCANCODE_D],
      key[SDL_SCANCODE_E],
      key[SDL_SCANCODE_Q]
    );

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffer);
    glVertexAttribPointer(glGetAttribLocation(program, "vertex_pos"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glUniform3fv(glGetUniformLocation(program, "cam_vec3"), 1, cam_vec3);
    glUniform4fv(glGetUniformLocation(program, "cam_quat"), 1, cam_quat);
    glUniformMatrix3fv(glGetUniformLocation(program, "cam_m3x3"), 1, GL_FALSE, cam_m3x3);
    glUniform4fv(glGetUniformLocation(program, "cam_proj"), 1, cam_proj);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(mesh) / sizeof(float) / 3);

    for (SDL_Event e; SDL_PollEvent(&e);) {
      if (e.type == SDL_QUIT)
        goto exit;
    }

    SDL_GL_SwapWindow(sdl_window);
    t_prev = t_curr;
  }

exit:;

  SDL_GL_DeleteContext(sdl_glcontext);
  SDL_Quit();
  return 0;
}

float GetAspectRatio(SDL_Window * sdl_window) {
  int w = 0;
  int h = 0;
  SDL_GetWindowSize(sdl_window, &w, &h);
  return w / (float)h;
}

void CreatePerspectiveProjection(
  float * proj,
  float aspect,
  float fov_y_rad,
  float n,
  float f)
{
  float d = 1.f / (float)tan(fov_y_rad * 0.5f);
  proj[0] = d / aspect;
  proj[1] = d;
  proj[2] = f / (n - f);
  proj[3] = (f * n) / (n - f);
}
