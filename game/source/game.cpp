#include "game.hpp"

#include "graphics.hpp"
#include "solaris/math/matrix.hpp"
#include "window.hpp"

#include <GLFW/glfw3.h>
#include <cmath>
#include <filesystem>
#include <fstream>
#include <iostream>

using solaris::Matrix4f;
using solaris::Vec3f;
using solaris::Vec4f;
using std::cout;
using std::endl;
using std::filesystem::path;
namespace matrix = solaris::matrix;

void GameLayer::setup(Handlers handlers) {
  handlers.addInstanceHandler<LoadEvent>(&GameLayer::onLoad);
  handlers.addInstanceHandler<RenderEvent>(&GameLayer::onRender);
}

std::string readFile(std::string_view pathStr) {
  path p{pathStr};
  std::stringstream stream;
  std::ifstream inputStream{p};
  stream << inputStream.rdbuf();
  return stream.str();
}

struct Vertex {
  Vec3f Position;
  Vec4f Color;
};

// clang-format off
static Vertex triangle[] {
  { .Position = { -1.0f, -1.0f, 0.0f }, .Color = { 1.0, 0.0, 0.0, 1.0 } },
  { .Position = {  1.0f, -1.0f, 0.0f }, .Color = { 0.0, 1.0, 0.0, 1.0 } },
  { .Position = {  0.0f,  1.0f, 0.0f }, .Color = { 0.0, 0.0, 1.0, 1.0 } },
};
// clang-format on

void GameLayer::onLoad(Context<LoadEvent> context) {
  cout << "Vendor:   " << glGetString(GL_VENDOR) << endl;
  cout << "Renderer: " << glGetString(GL_RENDERER) << endl;
  cout << "Verson:   " << glGetString(GL_VERSION) << endl;

  auto vertexShaderSource{readFile("triangle.vert")};
  auto fragmentShaderSource{readFile("triangle.frag")};

  m_Material = Material::builder()
                   .fragmentShader(fragmentShaderSource)
                   .vertexShader(vertexShaderSource)
                   .build();

  glGenVertexArrays(1, &m_VAO);
  glGenBuffers(1, &m_VBO);

  glBindVertexArray(m_VAO);

  glBindBuffer(GL_ARRAY_BUFFER, m_VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(triangle), triangle, GL_STATIC_DRAW);

  glVertexAttribPointer(
      0,
      3,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      static_cast<void *>(0)
  );
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(
      1,
      4,
      GL_FLOAT,
      GL_FALSE,
      sizeof(Vertex),
      reinterpret_cast<void *>(sizeof(Vec3f))
  );
  glEnableVertexAttribArray(1);

  glBindBuffer(GL_ARRAY_BUFFER, 0);
  glBindVertexArray(0);

  glGenBuffers(1, &m_UBO);
  glBindBuffer(GL_UNIFORM_BUFFER, m_UBO);
  glBufferData(GL_UNIFORM_BUFFER, sizeof(Matrix4f), nullptr, GL_DYNAMIC_DRAW);

  glBindBufferBase(GL_UNIFORM_BUFFER, 0, m_UBO);

  auto &window{context->get<Window>()};
  auto size{window.size()};
  auto scale{window.scale()};

  glViewport(
      0,
      0,
      int(float(size.x()) * scale.x()),
      int(float(size.y()) * scale.y())
  );

  context.next();
}

void GameLayer::onRender(Context<RenderEvent> context) {
  context.next();
  glClearColor(0.0, 0.011764706, 0.08627451, 1.0);
  glClear(GL_COLOR_BUFFER_BIT);

  auto time{glfwGetTime()};
  auto x{std::sin(time)};
  auto y{std::cos(time)};

  m_CameraPosition = {x, y, 0.0};
  auto scale{0.25f};

  auto projection{
      matrix::translation<float>(-m_CameraPosition) *
      matrix::scale<float>(scale)
  };

  glNamedBufferSubData(m_UBO, 0, sizeof(projection), &projection);

  glUseProgram(m_Material);
  glBindVertexArray(m_VAO);

  glDrawArrays(GL_TRIANGLES, 0, 3);
}
