#include "window.hpp"

#include <iostream>
#include <memory>

using solaris::Vec2i;
using solaris::Vec2f;
using std::cout;
using std::endl;

GLFWInitException::GLFWInitException(const char *message)
    : std::runtime_error(message) {}

GLFWLibrary::GLFWLibrary() {
  cout << "Initializing GLFW..." << endl;
  if (glfwInit() != GLFW_TRUE) {
    const char *message;
    glfwGetError(&message);

    throw GLFWInitException(message);
  }
}

GLFWLibrary::~GLFWLibrary() {
  cout << "Terminating GLFW..." << endl;
  glfwTerminate();
}

std::shared_ptr<GLFWLibrary> GLFWLibrary::getLibrary() {
  static std::weak_ptr<GLFWLibrary> librarySingleton{};

  if (!librarySingleton.expired()) {
    return librarySingleton.lock();
  }

  auto library{std::make_shared<GLFWLibrary>()};
  librarySingleton = library;
  return library;
}

Window Window::createWindow() {
  GLFW library{GLFWLibrary::getLibrary()};

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 4);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

  glfwWindowHint(GLFW_RESIZABLE, GLFW_FALSE);

  auto window{glfwCreateWindow(800, 600, "Hello, World!", nullptr, nullptr)};
  return Window{library, window};
}

bool Window::shouldClose() const {
  return glfwWindowShouldClose(m_Window) == GLFW_TRUE;
}

Vec2i Window::size() const {
  Vec2i size;
  glfwGetWindowSize(m_Window, &size.x(), &size.y());
  return size;
}

Vec2f Window::scale() const {
  Vec2f scale;
  glfwGetWindowContentScale(m_Window, &scale.x(), &scale.y());
  return scale;
}

Window::operator GLFWwindow *() const { return m_Window; }
