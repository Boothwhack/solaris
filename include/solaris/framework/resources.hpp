#pragma once
#include <algorithm>

namespace solaris::framework {
template <typename T>
class ResourceOwner {
  T m_Resource;

public:
  virtual ~ResourceOwner() = default;

  explicit ResourceOwner(T &&resource) : m_Resource{std::move(resource)} {}

  T &getResource() { return m_Resource; }
};

class Resources {
public:
  virtual ~Resources() = default;

  template <typename T>
  T &get() {
    return dynamic_cast<ResourceOwner<T> &>(*this).getResource();
  }
};

template <typename... Ts>
class ResourceOwners final : public Resources, public ResourceOwner<Ts>... {
public:
  explicit ResourceOwners(Ts &&...res)
      : Resources(), ResourceOwner<Ts>{std::move(res)}... {}

  template <typename T, typename... Args>
  ResourceOwners<T, Ts...> withResource(Args &&...args) {
    return ResourceOwners<T, Ts...>{
        std::move(T{std::forward<Args>(args)...}),
        std::move(ResourceOwner<Ts>::get())...,
    };
  }
};
} // namespace solaris::framework
