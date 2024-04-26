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

  bool operator==(const BasicObjectPtr<T> &other) const noexcept {
    return m_RootPtr == other.m_RootPtr;
  }
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

  [[nodiscard]] size_t getOffset() const { return m_Offset; }
};

template <typename... Ts>
class SelectiveObjectPtr : public BasicObjectPtr<SelectiveObjectPtr<Ts...>>,
                           FieldPtr<Ts>... {
public:
  SelectiveObjectPtr(uint8_t *rootPtr, const RuntimeStruct &runtimeStruct)
      : BasicObjectPtr<SelectiveObjectPtr>(rootPtr, runtimeStruct),
        FieldPtr<Ts>(runtimeStruct)... {}

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

  template <typename T, typename... Args>
  void emplaceField(Args &&...args) {
    T *fieldPtr{getFieldPtr<T>()};
    new (fieldPtr) T(std::forward<Args>(args)...);
  }

  template <typename T>
  T &getField() const {
    return *getFieldPtr<T>();
  }
};

class RawObjectPtr : BasicObjectPtr<RawObjectPtr> {
public:
  RawObjectPtr(uint8_t *rootPtr, const RuntimeStruct &runtimeStruct)
      : BasicObjectPtr(rootPtr, runtimeStruct) {}

  template <typename... Ts>
  SelectiveObjectPtr<Ts...> select() const {
    return {rootPtr(), runtimeStruct()};
  }
};
}
