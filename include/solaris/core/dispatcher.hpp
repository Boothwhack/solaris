#pragma once

#include <functional>
#include <type_traits>
#include <vector>

namespace solaris::core {
template <typename C>
class BaseDispatcher {
public:
  virtual ~BaseDispatcher() {}
};

template <typename E, typename C>
class Dispatcher : public BaseDispatcher<C> {
public:
  class Context;

  using Handler = std::function<void(Context)>;

  class Context {
    using Iterator = typename std::vector<Handler>::iterator;

    const E &m_Event;
    Iterator m_Iter;
    Iterator m_End;
    C &m_Context;

  public:
    Context(const E &event, Iterator iter, Iterator end, C &context)
        : m_Event{event}, m_Iter{iter}, m_End{end}, m_Context{context} {}

    void next() {
      if (m_Iter == m_End)
        return;

      auto &handler{*m_Iter};

      Context next{
          m_Event,
          m_Iter + 1,
          m_End,
          m_Context,
      };
      m_Iter = m_End;
      handler(next);
    }

    const E &event() const { return m_Event; }

    C *operator->() { return &m_Context; }

    C &operator*() { return m_Context; }
  };

private:
  std::vector<Handler> m_Handlers{};

public:
  void addHandler(Handler &&handler) {
    m_Handlers.push_back(std::move(handler));
  }

  void dispatch(const E &event, C &context) {
    Context contextWrapper{
        event,
        m_Handlers.begin(),
        m_Handlers.end(),
        context,
    };
    contextWrapper.next();
  }
};
} // namespace solaris::framework
