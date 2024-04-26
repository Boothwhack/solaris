#pragma once

#include <solaris/framework/allocation.hpp>
#include <solaris/framework/runtime_object.hpp>
#include <solaris/framework/runtime_struct.hpp>

namespace solaris {
class RuntimeVector {
  RuntimeStruct m_RuntimeStruct;
  Allocation m_Allocation;
  size_t m_Capacity;
  size_t m_Size;

public:
  template <typename... Ts>
  class View {
    RuntimeVector &m_Vector;

  public:
    explicit View(RuntimeVector &vector) : m_Vector{vector} {}

    SelectiveObjectPtr<Ts...> begin() const {
      return {(uint8_t *)m_Vector.m_Allocation, m_Vector.m_RuntimeStruct};
    }

    SelectiveObjectPtr<Ts...> end() const {
      auto endOffset{m_Vector.m_Size * m_Vector.m_RuntimeStruct.Stride};
      return {
          endOffset + (uint8_t *)m_Vector.m_Allocation,
          m_Vector.m_RuntimeStruct
      };
    }
  };

  explicit RuntimeVector(RuntimeStruct runtimeStruct)
      : m_RuntimeStruct{std::move(runtimeStruct)}, m_Allocation{0},
        m_Capacity{0}, m_Size{0} {}

private:
  void ensureCapacity(size_t requested) {
    if (m_Capacity >= requested)
      return;

    size_t newCapacity = m_Capacity;
    if (newCapacity == 0)
      newCapacity = 1;

    while (newCapacity < requested)
      newCapacity *= 2;

    Allocation newAllocation{newCapacity * m_RuntimeStruct.Stride};

    for (size_t i{0}; i < m_Size; ++i) {
      auto offset{i * m_RuntimeStruct.Stride};

      auto source{offset + (uint8_t *)m_Allocation};
      auto destination{offset + (uint8_t *)newAllocation};

      for (const auto &member : m_RuntimeStruct.Members) {
        member.Field.MoveFunction(
            reinterpret_cast<void *>(source + member.Offset),
            reinterpret_cast<void *>(destination + member.Offset)
        );
      }
    }

    m_Allocation = std::move(newAllocation);
    m_Capacity = newCapacity;
  }

  [[nodiscard]] uint8_t *uncheckedGet(size_t index) const noexcept {
    return index * m_RuntimeStruct.Stride + (uint8_t *)m_Allocation;
  }

public:
  RawObjectPtr pushBack() {
    ensureCapacity(m_Size + 1);

    auto ptr{uncheckedGet(m_Size)};
    m_Size += 1;

    return {ptr, m_RuntimeStruct};
  }

  RawObjectPtr operator[](size_t index) {
    auto ptr{uncheckedGet(index)};
    return {ptr, m_RuntimeStruct};
  }

  size_t size() const { return m_Size; }

  size_t capacity() const { return m_Capacity; }
};
} // namespace solaris
