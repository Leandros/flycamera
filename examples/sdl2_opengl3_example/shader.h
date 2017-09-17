#pragma once

static inline GLuint CreateShader(GLenum shader_type, const char * shader_filepath) {
  GLuint shader = glCreateShader(shader_type);

  FILE * fd = fopen(shader_filepath, "rb");

  if (fd == NULL) {
    fprintf(stderr, "Can't open file: %s\n", shader_filepath);
    return 0;
  }

  fseek(fd, 0, SEEK_END);
  long int bytes = ftell(fd);
  fseek(fd, 0, SEEK_SET);

  char src[bytes + 1];
  src[bytes] = 0;
  fread(&src, bytes, 1, fd);

  fclose(fd);

  char * shader_str = &src[0];
  glShaderSource(shader, 1, (const char **)&shader_str, NULL);

  glCompileShader(shader);

  GLint status = 0;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint length;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &length);
    GLchar log[length];
    glGetShaderInfoLog(shader, length, NULL, log);
    char shader_type_str[8];
    switch (shader_type) {
      case GL_VERTEX_SHADER:   sprintf(shader_type_str, "vertex");   break;
      case GL_FRAGMENT_SHADER: sprintf(shader_type_str, "fragment"); break;
    }
    fprintf(stderr, "Compile error in %s shader:\n%s\n", shader_type_str, log);
    return 0;
  }

  return shader;
}

static inline GLuint CreateProgram(const char * vert_filepath, const char * frag_filepath) {
  GLuint vert = CreateShader(GL_VERTEX_SHADER, vert_filepath);
  GLuint frag = CreateShader(GL_FRAGMENT_SHADER, frag_filepath);
  
  GLuint program = glCreateProgram();
  
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  
  glLinkProgram(program);
  
  GLint status = 0;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint length;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &length);
    GLchar log[length];
    glGetProgramInfoLog(program, length, NULL, log);
    printf("Program link error: %s\n", log);
    return 0;
  }
  
  // glDetachShader(program, vert);
  // glDetachShader(program, frag);

  // glDeleteShader(vert);
  // glDeleteShader(frag);
  
  return program;
}
