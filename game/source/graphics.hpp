#pragma once

#include <stdexcept>
#include <string_view>
#include <vector>

using GLIdentifier = unsigned int;

class ShaderCompileError : public std::runtime_error {
public:
  template <typename... Args>
  ShaderCompileError(Args &&...args)
      : std::runtime_error{std::forward<Args &&>(args)...} {}
};

class ProgramLinkError : public std::runtime_error {
public:
  template <typename... Args>
  ProgramLinkError(Args &&...args)
      : std::runtime_error{std::forward<Args &&>(args)...} {}
};

class Shader {
  GLIdentifier m_Shader;

public:
  explicit Shader(GLIdentifier shader);
  Shader(Shader &&);
  Shader(const Shader &) = delete;
  ~Shader();

  Shader &operator=(Shader &&) noexcept;
  Shader &operator=(const Shader &) = delete;

  static Shader compile(GLIdentifier type, std::string_view source);

  operator GLIdentifier() const;
};

class Material {
  GLIdentifier m_Program;

public:
  class Builder {
    std::vector<Shader> m_Shaders;

  public:
    Builder() = default;

    Builder &fragmentShader(std::string_view shader);
    Builder &vertexShader(std::string_view shader);

    [[nodiscard]] Material build() const;
  };

  Material();
  Material(GLIdentifier id);
  Material(Material&&) noexcept;
  Material(const Material&) = delete;

  ~Material();

  Material& operator=(Material&&) noexcept;
  Material& operator=(const Material&) = delete;

  static Builder builder();

  operator GLIdentifier() const;
};
