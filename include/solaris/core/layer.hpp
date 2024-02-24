#pragma once

#include "bus.hpp"
#include "dispatcher.hpp"

namespace solaris::core {
template <typename C>
class BaseLayer {
public:
  virtual ~BaseLayer() {}
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
    m_Bus.template addHandler<E>([=](typename Dispatcher<E, C>::Context context
                                 ) { (m_Layer.*handler)(std::move(context)); });
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

  using LayerHandlers = LayerHandlers<T, C>;

  virtual ~Layer() {}

  virtual void setup(LayerHandlers handlers) {}
};
} // namespace solaris::framework
