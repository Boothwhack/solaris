#pragma once

#include "bus.hpp"
#include <any>
#include <mutex>
#include <optional>
#include <queue>
#include <stack>

namespace solaris::core {
template <typename C>
class Queue {
  class QueuedEvent {
    std::any m_Event;
    void (*m_Dispatch)(Bus<C> &, C &, const std::any &);

    template <typename T>
    static void dispatcher(Bus<C> &bus, C &context, const std::any &event) {
      if (const T * castEvent{std::any_cast<T>(&event)}) {
        bus.dispatch(*castEvent, context);
      }
    }

    QueuedEvent(std::any &&event, typeof(m_Dispatch) dispatch)
        : m_Event{std::move(event)}, m_Dispatch{dispatch} {}

  public:
    template <typename T>
    static QueuedEvent newFor(std::any &&event) {
      return QueuedEvent{std::move(event), &QueuedEvent::dispatcher<T>};
    }

    void dispatch(Bus<C> &bus, C &context) {
      m_Dispatch(bus, context, m_Event);
    }
  };

  std::queue<QueuedEvent> m_Events{};
  std::mutex m_Lock{};

  std::optional<QueuedEvent> popEvent() {
    std::lock_guard lockGuard{m_Lock};

    if (m_Events.empty())
      return std::nullopt;

    auto event{std::move(m_Events.front())};
    m_Events.pop();
    return std::move(event);
  }

public:
  template <typename T, typename... Args>
  void enqueue(Args &&...args) {
    std::lock_guard lockGuard{m_Lock};

    m_Events.push(QueuedEvent::template newFor<T>(
        std::make_any<T>(std::forward<Args>(args)...)
    ));
  }

  void dispatchOn(Bus<C> &bus, C &context) {
    while (auto event{popEvent()}) {
      event->dispatch(bus, context);
    }
  }
};
} // namespace solaris::core