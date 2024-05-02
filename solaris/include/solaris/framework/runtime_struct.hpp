#pragma once

#include <cstddef>
#include <set>
#include <typeindex>
#include <typeinfo>
#include <utility>
#include <vector>

namespace solaris {
/** Runtime information for a field type. */
struct RuntimeField {
  using CopyFunctionPtr = void (*)(void *source, void *destination);
  using DestructorFunctionPtr = void (*)(void *ptr);
  using MoveFunctionPtr = void (*)(void *source, void *destination);

  std::type_index TypeIndex;
  size_t Size;
  size_t Alignment;
  CopyFunctionPtr CopyFunction;
  DestructorFunctionPtr DestructorFunction;
  MoveFunctionPtr MoveFunction;

  template <typename T>
  static RuntimeField runtimeFieldFor() {
    return {
        .TypeIndex = typeid(T),
        .Size = sizeof(T),
        .Alignment = alignof(T),
        .CopyFunction = &RuntimeField::basicCopyFunction<T>,
        .DestructorFunction = &RuntimeField::basicDestructorFunction<T>,
        .MoveFunction = &RuntimeField::basicMoveFunction<T>,
    };
  }

  std::strong_ordering operator<=>(const RuntimeField &other) const {
    return TypeIndex.operator<=>(other.TypeIndex);
  }

private:
  template <typename T>
  static void basicCopyFunction(void *source, void *destination) {
    T *sourcePtr{reinterpret_cast<T *>(source)};
    const T &sourceRef{*sourcePtr};
    new (destination) T(sourceRef);
  }

  template <typename T>
  static void basicDestructorFunction(void *ptr) {
    T *tPtr{reinterpret_cast<T *>(ptr)};
    tPtr->~T();
  }

  template <typename T>
  static void basicMoveFunction(void *source, void *destination) {
    T *sourcePtr{reinterpret_cast<T *>(source)};
    T &sourceRef{*sourcePtr};
    new (destination) T(std::move(sourceRef));
    sourcePtr->~T();
  }
};

struct RuntimeStruct {
  struct Member {
    RuntimeField Field;
    size_t Offset;
  };

  std::vector<Member> Members;
  size_t Size{0};
  size_t Stride{0};
  size_t Alignment{1};

  RuntimeStruct() = default;

  explicit RuntimeStruct(const std::set<RuntimeField> &fields) : Members() {
    size_t offset{0};
    size_t maxAlignment{0};

    Members.reserve(fields.size());

    for (const RuntimeField &field : fields) {
      if (offset % field.Alignment != 0) {
        auto alignmentOffset{field.Alignment - offset % field.Alignment};
        offset += alignmentOffset;
      }
      Members.push_back({.Field = field, .Offset = offset});

      offset += field.Size;
      maxAlignment = std::max(maxAlignment, field.Alignment);
    }

    Size = offset;
    Alignment = maxAlignment;
    Stride = Size + (Size % Alignment != 0 ? Alignment - Size % Alignment : 0);
  }

  template <typename T>
  [[nodiscard]] RuntimeStruct withMember() const {
    return withField(RuntimeField::runtimeFieldFor<T>());
  }

  [[nodiscard]] RuntimeStruct withField(RuntimeField field) const {
    std::set<RuntimeField> fields;
    for (const auto &member : Members) {
      fields.insert(member.Field);
    }
    fields.insert(field);
    return RuntimeStruct{fields};
  }

  template<typename... Ts>
  static RuntimeStruct withMembers() {
    std::set<RuntimeField> fields{RuntimeField::runtimeFieldFor<Ts>()...};
    return RuntimeStruct{fields};
  }
};
} // namespace solaris

template <>
struct std::hash<solaris::RuntimeField> {
  size_t operator()(const solaris::RuntimeField &field) const noexcept {
    return field.TypeIndex.hash_code();
  }
};
