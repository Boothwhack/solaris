#include "graphics.hpp"

#include <glad/gl.h>
#include <iostream>

using Builder = Material::Builder;

Shader::Shader(GLuint shader) : m_Shader{shader} {}

Shader::Shader(Shader &&other) : m_Shader{other.m_Shader} {
  other.m_Shader = 0;
}

Shader &Shader::operator=(Shader &&rhs) noexcept {
  m_Shader = rhs.m_Shader;
  rhs.m_Shader = 0;
  return *this;
}

Shader::~Shader() { glDeleteShader(m_Shader); }

Shader Shader::compile(GLenum type, std::string_view source) {
  auto shader{glCreateShader(type)};

  const char *sources[]{source.data()};
  GLint lengths[]{static_cast<int>(source.length())};

  glShaderSource(shader, 1, sources, lengths);
  glCompileShader(shader);

  GLint status;
  glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
  if (status == GL_FALSE) {
    GLint logLength;
    glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &logLength);

    std::string message;
    message.resize(logLength);
    glGetShaderInfoLog(shader, logLength, nullptr, message.data());

    throw ShaderCompileError(message);
  }

  return Shader{shader};
}

Shader::operator GLuint() const { return m_Shader; }

Material::Material() : m_Program(0) {}

Material::Material(GLuint id) : m_Program(id) {}

Material::Material(Material &&other) noexcept : m_Program{other.m_Program} {
  other.m_Program = 0;
}

Material::~Material() { glDeleteProgram(m_Program); }

Material &Material::operator=(Material &&rhs) noexcept {
  m_Program = rhs.m_Program;
  rhs.m_Program = 0;
  return *this;
}

Builder &Builder::fragmentShader(std::string_view shader) {
  m_Shaders.push_back(Shader::compile(GL_FRAGMENT_SHADER, shader));
  return *this;
}

Builder &Builder::vertexShader(std::string_view shader) {
  m_Shaders.push_back(Shader::compile(GL_VERTEX_SHADER, shader));
  return *this;
}

Material Builder::build() const {
  auto program{glCreateProgram()};
  for (const auto &shader : m_Shaders) {
    glAttachShader(program, shader);
  }

  glLinkProgram(program);
  GLint status;
  glGetProgramiv(program, GL_LINK_STATUS, &status);
  if (status == GL_FALSE) {
    GLint logLength;
    glGetProgramiv(program, GL_INFO_LOG_LENGTH, &logLength);

    std::string message;
    message.resize(logLength);
    glGetProgramInfoLog(program, logLength, nullptr, message.data());

    throw ProgramLinkError(message);
  }

  return Material{program};
}

Builder Material::builder() { return {}; }

Material::operator GLuint() const { return m_Program; }
