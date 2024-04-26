#pragma

#include <cstddef>

namespace solaris {
class Allocation {
  void *m_Ptr;

public:
  explicit Allocation(size_t size) : m_Ptr{size > 0 ? malloc(size) : nullptr} {}
  Allocation(const Allocation &) = delete;
  Allocation(Allocation &&other) noexcept {
    m_Ptr = other.m_Ptr;
    other.m_Ptr = nullptr;
  }

  Allocation &operator=(Allocation &&other) noexcept {
    free(m_Ptr);
    m_Ptr = other.m_Ptr;
    other.m_Ptr = nullptr;
    return *this;
  }

  virtual ~Allocation() { free(m_Ptr); }

  [[nodiscard]] void *get() const { return m_Ptr; }

  template <typename T>
  explicit operator T *() const {
    return reinterpret_cast<T *>(m_Ptr);
  }
};
} // namespace solaris
