#pragma once

#include "dispatcher.hpp"
#include <memory>
#include <typeindex>
#include <unordered_map>

namespace solaris::framework {
template <typename C>
class Bus {
  std::unordered_map<std::type_index, std::unique_ptr<BaseDispatcher<C>>>
      m_Dispatchers{};

public:
  template <typename E>
  Dispatcher<E, C> &getDispatcherFor() {
    if (auto it{m_Dispatchers.find(typeid(E))}; it != m_Dispatchers.end()) {
      return dynamic_cast<Dispatcher<E, C> &>(*it->second);
    }
    auto [it, inserted]{
        m_Dispatchers.insert({typeid(E), std::make_unique<Dispatcher<E, C>>()}
        )};
    // BaseDispatcher<C>& dispatcher{*m_Dispatchers[typeid(E)] =
    // std::make_unique<Dispatcher<E, C>>()}; auto [it,
    // inserted]{m_Dispatchers.try_emplace(typeid(E))};
    return dynamic_cast<Dispatcher<E, C> &>(*it->second);

    // return dynamic_cast<Dispatcher<E, C>&>(dispatcher);
  }

  template <typename E>
  void dispatch(const E &event, C &context) {
    if (auto it{m_Dispatchers.find(typeid(E))}; it != m_Dispatchers.end()) {
      auto &dispatcher{dynamic_cast<Dispatcher<E, C> &>(*it->second)};
      dispatcher.dispatch(event, context);
    }
  }
};
} // namespace solaris::framework
