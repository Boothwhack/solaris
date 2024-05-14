#pragma once

#include <glad/gl.h>

#include <GLFW/glfw3.h>

#include <memory>
#include <solaris/math/matrix.hpp>
#include <utility>

class GLFWInitException : std::runtime_error {
public:
  explicit GLFWInitException(const char *message);
};

class GLFWLibrary {
public:
  GLFWLibrary();
  ~GLFWLibrary();

  static std::shared_ptr<GLFWLibrary> getLibrary();
};

using GLFW = std::shared_ptr<GLFWLibrary>;

class Window {
  GLFW m_Library;
  GLFWwindow *m_Window;

  Window(GLFW library, GLFWwindow *window)
      : m_Library{std::move(library)}, m_Window(window) {}

public:
  static Window createWindow();

  bool shouldClose() const;

  solaris::Vec2i size() const;

  solaris::Vec2f scale() const;

  operator GLFWwindow *() const;
};
