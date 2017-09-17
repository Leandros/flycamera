#include "../../flycamera.h"

#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include "suzanne.h"
#include "shader.h"
#include "../glad/src/glad.c"

float GetAspectRatio(GLFWwindow * glfw_window);
void CreatePerspectiveProjection(float * proj, float aspect, float fov_y_rad, float n, float f);

int main() {
  glfwInit();
  
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);

  GLFWwindow * glfw_window = glfwCreateWindow(
    1280, 720, "FlyCamera: GLFW",
    NULL, NULL
  );

  glfwMakeContextCurrent(glfw_window);

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

  glClearColor(222 / 255.f, 143 / 255.f, 165 / 255.f, 1.f);
  glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

  float cam_vec3[3] = {0.f};
  float cam_quat[4] = {0.f, 0.f, 0.f, 1.f};
  float cam_m3x3[9] = {0.f};
  float cam_proj[4] = {0.f};

  CreatePerspectiveProjection(
    cam_proj,
    GetAspectRatio(glfw_window),
    85.0f * 0.017453f,
    0.01f, 1000.f
  );

  glfwSetInputMode(glfw_window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

  double t_prev = glfwGetTime();

  while (!glfwWindowShouldClose(glfw_window)) {
    double t_curr = glfwGetTime();
    double dt = (t_curr - t_prev) * 60.0;

    glfwPollEvents();
    double mouse_x = 0;
    double mouse_y = 0;
    static double mouse_x_prev = 0;
    static double mouse_y_prev = 0;
    glfwGetCursorPos(glfw_window, &mouse_x, &mouse_y);

    flycamera(
      cam_vec3,
      cam_quat,
      cam_m3x3,
      0.05f,
      0.05f * dt,
      mouse_x_prev - mouse_x,
      mouse_y_prev - mouse_y,
      glfwGetKey(glfw_window, GLFW_KEY_W),
      glfwGetKey(glfw_window, GLFW_KEY_A),
      glfwGetKey(glfw_window, GLFW_KEY_S),
      glfwGetKey(glfw_window, GLFW_KEY_D),
      glfwGetKey(glfw_window, GLFW_KEY_E),
      glfwGetKey(glfw_window, GLFW_KEY_Q)
    );

    mouse_x_prev = mouse_x;
    mouse_y_prev = mouse_y;

    glClear(GL_COLOR_BUFFER_BIT);
    glUseProgram(program);
    glBindBuffer(GL_ARRAY_BUFFER, mesh_buffer);
    glVertexAttribPointer(glGetAttribLocation(program, "vertex_pos"), 3, GL_FLOAT, GL_FALSE, 0, NULL);
    glUniform3fv(glGetUniformLocation(program, "cam_vec3"), 1, cam_vec3);
    glUniform4fv(glGetUniformLocation(program, "cam_quat"), 1, cam_quat);
    glUniformMatrix3fv(glGetUniformLocation(program, "cam_m3x3"), 1, GL_FALSE, cam_m3x3);
    glUniform4fv(glGetUniformLocation(program, "cam_proj"), 1, cam_proj);
    glDrawArrays(GL_TRIANGLES, 0, sizeof(mesh) / sizeof(float) / 3);

    glfwSwapBuffers(glfw_window);
    t_prev = t_curr;
  }

  glfwDestroyWindow(glfw_window);
  glfwTerminate();
  return 0;
}

float GetAspectRatio(GLFWwindow * glfw_window) {
  int w = 0;
  int h = 0;
  glfwGetWindowSize(glfw_window, &w, &h);
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
