#pragma once

#include "graphics.hpp"
#include "solaris/core/layer.hpp"
#include "solaris/framework/resources.hpp"
#include "solaris/math/matrix.hpp"

struct LoadEvent {};

struct RenderEvent {};

class GameLayer
    : public solaris::core::Layer<GameLayer, solaris::framework::Resources> {
  Material m_Material;
  solaris::Vec3f m_CameraPosition;
  GLIdentifier m_ShaderProgram;
  GLIdentifier m_VAO;
  GLIdentifier m_VBO;
  GLIdentifier m_UBO;

public:
  GameLayer() = default;

  void setup(Handlers handlers) override;

  void onLoad(Context<LoadEvent> context);

  void onRender(Context<RenderEvent> context);
};
