#pragma once

#include "layer.hpp"
#include <functional>
#include <memory>
#include <vector>

namespace solaris::core {
template <typename C>
class LayerStack {
  template <typename T>
  struct TypeKey {};

  struct ManagedLayer {
    std::unique_ptr<BaseLayer<C>> m_Layer;
    void (*m_Setup)(Bus<C> &, BaseLayer<C> &);

    template <typename T>
    static void setupLayer(Bus<C> &bus, BaseLayer<C> &layer) {
      T &castLayer{dynamic_cast<T &>(layer)};
      castLayer.setup(LayerHandlers<T, C>{bus, castLayer});
    }

  public:
    template <typename T, typename... Args>
    ManagedLayer(std::unique_ptr<T> layer)
        : m_Layer{std::move(layer)}, m_Setup{&setupLayer<T>} {}

    void setup(Bus<C> &bus) { m_Setup(bus, *m_Layer); }
  };

  std::vector<ManagedLayer> m_Layers{};

public:
  template <typename T, typename... Args>
  void addLayer(Args &&...args) {
    m_Layers.emplace_back(std::make_unique<T>(std::forward<Args>(args)...));
  }

  Bus<C> compileBus() {
    Bus<C> bus{};
    for (auto &layer : m_Layers) {
      layer.setup(bus);
    }
    return bus;
  }
};
} // namespace solaris::framework
