#pragma once

#include <cstdint>
#include <solaris/framework/runtime_struct.hpp>
#include <stdexcept>

namespace solaris {
template <typename T>
class BasicObjectPtr {
  uint8_t *m_RootPtr;
  const RuntimeStruct &m_Struct;

protected:
  [[nodiscard]] uint8_t *rootPtr() const { return m_RootPtr; }
  [[nodiscard]] const RuntimeStruct &runtimeStruct() const { return m_Struct; }

  BasicObjectPtr(uint8_t *rootPtr, const RuntimeStruct &runtimeStruct)
      : m_RootPtr{rootPtr}, m_Struct{runtimeStruct} {}
  virtual ~BasicObjectPtr() = default;

public:
  using difference_type = ptrdiff_t;

  T &operator++() {
    m_RootPtr += m_Struct.Stride;
    return dynamic_cast<T &>(*this);
  }

  T operator++(int) {
    const T &self{*this};
    T copy{self};
    ++copy;
    return copy;
  }

  bool operator==(const BasicObjectPtr &other) const noexcept {
    return m_RootPtr == other.m_RootPtr;
  }

  bool operator==(nullptr_t) const noexcept { return m_RootPtr == nullptr; }
};

template <typename T>
class FieldPtr {
  size_t m_Offset;

public:
  explicit FieldPtr(const RuntimeStruct &runtimeStruct) {
    auto memberIt{std::ranges::find_if(
        runtimeStruct.Members,
        [](const auto &member) { return member.Field.TypeIndex == typeid(T); }
    )};
    if (memberIt == runtimeStruct.Members.end()) {
      throw std::runtime_error("could not find field in runtime struct");
    }

    m_Offset = memberIt->Offset;
  }

  explicit FieldPtr(size_t offset) : m_Offset{offset} {}

  [[nodiscard]] size_t getOffset() const { return m_Offset; }
};

template <typename... Ts>
class SelectiveObjectRef;

template <typename... Ts>
class SelectiveObjectPtr : public BasicObjectPtr<SelectiveObjectPtr<Ts...>>,
                           FieldPtr<Ts>... {
public:
  SelectiveObjectPtr()
      : BasicObjectPtr<SelectiveObjectPtr>(
            nullptr,
            RuntimeStruct::withMembers<Ts...>()
        ),
        FieldPtr<Ts>(
            BasicObjectPtr<SelectiveObjectPtr>::rootPtr(),
            BasicObjectPtr<SelectiveObjectPtr>::runtimeStruct()
        )... {}
  SelectiveObjectPtr(uint8_t *rootPtr, const RuntimeStruct &runtimeStruct)
      : BasicObjectPtr<SelectiveObjectPtr>(rootPtr, runtimeStruct),
        FieldPtr<Ts>(runtimeStruct)... {}

  SelectiveObjectPtr(SelectiveObjectPtr &&rhs) noexcept
      : BasicObjectPtr<SelectiveObjectPtr>{rhs.rootPtr(), rhs.runtimeStruct()},
        FieldPtr<Ts>(std::move(rhs))... {}
  SelectiveObjectPtr(const SelectiveObjectPtr &rhs) noexcept
      : BasicObjectPtr<SelectiveObjectPtr>(rhs.rootPtr(), rhs.runtimeStruct()),
        FieldPtr<Ts>(rhs)... {}

  using value_type = SelectiveObjectRef<Ts...>;

  SelectiveObjectPtr &operator=(SelectiveObjectPtr &&rhs) noexcept {
    new (this) SelectiveObjectPtr(std::move(rhs));
    return *this;
  }
  SelectiveObjectPtr &operator=(const SelectiveObjectPtr &rhs) noexcept {
    new (this) SelectiveObjectPtr(rhs);
    return *this;
  }

  template <typename T>
  [[nodiscard]] size_t getFieldOffset() const {
    return FieldPtr<T>::getOffset();
  }

  template <typename T>
  [[nodiscard]] T *getFieldPtr() const {
    auto offsetPtr{
        BasicObjectPtr<SelectiveObjectPtr>::rootPtr() + FieldPtr<T>::getOffset()
    };
    return reinterpret_cast<T *>(offsetPtr);
  }

  SelectiveObjectRef<Ts...> operator*() const;
  SelectiveObjectRef<Ts...> operator->() const;
};

template <typename... Ts>
class SelectiveObjectRef : FieldPtr<Ts>... { // SelectiveObjectPtr<Ts...> {
  uint8_t *m_Ptr;

public:
  explicit
  SelectiveObjectRef(uint8_t *ptr, const SelectiveObjectPtr<Ts...> &rhs)
      : m_Ptr{ptr}, FieldPtr<Ts>(rhs.template getFieldOffset<Ts>())... {}

  SelectiveObjectRef *operator->() { return this; }

  template <typename T, typename... Args>
  void emplaceField(Args &&...args) {
    // T *fieldPtr{SelectiveObjectPtr<Ts...>::template getFieldPtr<T>()};

    new (m_Ptr + FieldPtr<T>::getOffset()) T(std::forward<Args>(args)...);
  }

  template <typename T>
  T &getField() const {
    return *reinterpret_cast<T *>(m_Ptr + FieldPtr<T>::getOffset());
  }
};

template <typename... Ts>
SelectiveObjectRef<Ts...> SelectiveObjectPtr<Ts...>::operator*() const {
  return SelectiveObjectRef<Ts...>{BasicObjectPtr<SelectiveObjectPtr>::rootPtr(), *this};
}

template <typename... Ts>
SelectiveObjectRef<Ts...> SelectiveObjectPtr<Ts...>::operator->() const {
  return SelectiveObjectRef<Ts...>{BasicObjectPtr<SelectiveObjectPtr>::rootPtr(), *this};
}

class RawObjectPtr : public BasicObjectPtr<RawObjectPtr> {
public:
  RawObjectPtr(nullptr_t) : RawObjectPtr(nullptr, RuntimeStruct()) {}

  RawObjectPtr(uint8_t *rootPtr, const RuntimeStruct &runtimeStruct)
      : BasicObjectPtr(rootPtr, runtimeStruct) {}

  template <typename... Ts>
  SelectiveObjectPtr<Ts...> select() const {
    return {rootPtr(), runtimeStruct()};
  }
};
} // namespace solaris
