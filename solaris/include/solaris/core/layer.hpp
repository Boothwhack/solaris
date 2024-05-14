#pragma once

#include "bus.hpp"
#include "dispatcher.hpp"

namespace solaris::core {
template <typename C>
class BaseLayer {
public:
  virtual ~BaseLayer() = default;
};

template <typename T, typename C>
class LayerHandlers {
  Bus<C> &m_Bus;
  T &m_Layer;

public:
  LayerHandlers(Bus<C> &bus, T &layer) : m_Bus{bus}, m_Layer{layer} {}

  template <typename E>
  void addInstanceHandler(void (T::*handler)(typename Dispatcher<E, C>::Context)
  ) {
    T& layer{m_Layer};
    m_Bus.template addHandler<E>([&layer, handler=handler](typename Dispatcher<E, C>::Context context
                                 ) { (layer.*handler)(std::move(context)); });
  }

  template <typename E>
  void addStaticHandler(void (*handler)(typename Dispatcher<E, C>::Context)) {
    m_Bus.template addHandler<E>(handler);
  }
};

template <typename T, typename C>
class Layer : public BaseLayer<C> {
public:
  template <typename E>
  using Context = typename Dispatcher<E, C>::Context;

  using Handlers = LayerHandlers<T, C>;

  virtual ~Layer() {}

  virtual void setup(Handlers handlers) {}
};
} // namespace solaris::core
