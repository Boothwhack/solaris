#include "game.hpp"
#include "graphics.hpp"
#include "solaris/core/layer_stack.hpp"
#include "solaris/framework/resources.hpp"
#include "window.hpp"

using solaris::core::LayerStack;
using solaris::framework::Resources;
using solaris::framework::ResourceOwners;

#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include <iostream>

int main() {
  auto window{Window::createWindow()};

  glfwMakeContextCurrent(window);

  // glad: load all OpenGL function pointers
  // ---------------------------------------
  if (!gladLoadGL(glfwGetProcAddress)) {
    std::cout << "Failed to initialize GLAD" << std::endl;
    return -1;
  }

  auto resources{ResourceOwners().withResource<Window>(std::move(window))};

  LayerStack<Resources> layers;
  layers.addLayer<GameLayer>();

  auto bus{layers.compileBus()};

  bus.dispatch(LoadEvent(), resources);

  while (!window.shouldClose()) {
    bus.dispatch(RenderEvent(), resources);

    glfwSwapBuffers(window);
    glfwPollEvents();
  }
}

/*#include <iostream>

#include "game.hpp"
#include "solaris/core/layer_stack.hpp"
#include "window.hpp"

using solaris::core::LayerStack;
using solaris::framework::Resources;
using solaris::framework::ResourceOwners;

int main() {
  auto resources{ResourceOwners().withResource<Window>(std::move(window))};

  LayerStack<Resources> layers;
  layers.addLayer<GameLayer>();

  auto bus{layers.compileBus()};

  glfwShowWindow(window);
  glfwMakeContextCurrent(window);

  gladLoadGL(glfwGetProcAddress);

  float x, y;
  glfwGetWindowContentScale(window, &x, &y);
  std::cout << x << " x " << y << std::endl;

  bus.dispatch(LoadEvent(), resources);

  while(!window.shouldClose()) {
    glfwPollEvents();

    glClearColor(0.0, 0.011764706, 0.08627451, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);

    bus.dispatch(RenderEvent(), resources);

    glfwSwapBuffers(window);
  }
}*/
