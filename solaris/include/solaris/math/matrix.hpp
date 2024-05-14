#pragma once

#include <array>
#include <concepts>
#include <cstddef>
#include <cstring>

namespace solaris {

namespace impl {
template <typename T>
concept Addable = requires(const T &a, const T &b) { a + b; };

template <Addable T>
T add(const T &a, const T &b) {
  return a + b;
}

template <typename T>
concept Subtractible = requires(const T &a, const T &b) { a - b; };

template <Subtractible T>
T subtract(const T &a, const T &b) {
  return a - b;
}
} // namespace impl

template <typename T, size_t R, size_t C>
class Matrix {
public:
  static constexpr size_t Columns{C};
  static constexpr size_t Rows{R};

private:
  std::array<T, R * C> m_Data{};

  template <std::invocable<const T &, const T &> F>
  Matrix piecewise(const Matrix &other, F &&f) const {
    Matrix output;

    for (size_t i{0}; i < m_Data.size(); ++i) {
      output.m_Data[i] = f(m_Data[i], other.m_Data[i]);
    }
    return output;
  }

public:
  Matrix() = default;
  template <typename... Args>
  Matrix(Args &&...args)
      : m_Data{(T)std::forward<Args>(args)...} {
  } // NOLINT(*-explicit-constructor)

  [[nodiscard]] size_t columns() const { return Columns; }
  [[nodiscard]] size_t rows() const { return Rows; }

  friend std::ostream &operator<<(std::ostream &os, const Matrix &obj) {
    os << "[ ";
    for (size_t c{0}; c < Columns; ++c) {
      os << "[ ";
      for (size_t r{0}; r < Rows - 1; ++r) {
        os << obj.at({r, c}) << ", ";
      }
      os << obj.at({Rows - 1, c});
      os << " ] ";
    }
    os << "]";
    return os;
  }

  bool operator==(const Matrix<T, R, C> &other) const {
    return std::memcmp(m_Data.data(), other.m_Data.data(), sizeof(m_Data)) == 0;
  }

  T &at(size_t index)
    requires(Columns == 1)
  {
    return m_Data[index];
  }

  [[nodiscard]] const T &at(size_t index) const
    requires(Columns == 1)
  {
    return m_Data[index];
  }

  T &at(std::array<size_t, 2> cell) {
    auto index{cell[0] + cell[1] * Columns};
    return m_Data[index];
  }

  [[nodiscard]] const T &at(std::array<size_t, 2> cell) const {
    auto index{cell[0] + cell[1] * Columns};
    return m_Data[index];
  }

  T &operator[](std::array<size_t, 2> cell) { return at(cell); }

  const T &operator[](std::array<size_t, 2> cell) const { return at(cell); }

  T &operator[](size_t index)
    requires(Columns == 1)
  {
    return at(index);
  }

  const T &operator[](size_t index) const
    requires(Columns == 1)
  {
    return at(index);
  }

  T &x()
    requires(Columns == 1)
  {
    return at(0);
  }

  [[nodiscard]] const T &x() const
    requires(Columns == 1)
  {
    return at(0);
  }

  T &y()
    requires(Columns == 1 && Rows > 1)
  {
    return at(1);
  }

  [[nodiscard]] const T &y() const
    requires(Columns == 1 && Rows > 1)
  {
    return at(1);
  }

  T &z()
    requires(Columns == 1 && Rows > 2)
  {
    return at(2);
  }

  [[nodiscard]] const T &z() const
    requires(Columns == 1 && Rows > 2)
  {
    return at(2);
  }

  T &w()
    requires(Columns == 1 && Rows > 3)
  {
    return at(3);
  }

  [[nodiscard]] const T &w() const
    requires(Columns == 1 && Rows > 3)
  {
    return at(4);
  }

  template <size_t R2, size_t C2>
  Matrix<T, R2, C2>
  subMatrix(const Matrix<size_t, 2, 1> &offset = {0u, 0u}) const {
    Matrix<T, R2, C2> output;
    for (size_t row{0}; row < R2; ++row) {
      for (size_t column{0}; column < C2; ++column) {
        output[{column, row}] = at({column + offset.x(), row + offset.y()});
      }
    }
    return output;
  }

  Matrix<T, R, 1> column(size_t c) const { return subMatrix<R, 1>({c, 0}); }

  Matrix<T, 1, C> row(size_t r) const { return subMatrix<1, C>({0, r}); }

  [[nodiscard]] Matrix add(const Matrix &other) const
    requires(impl::Addable<T>)
  {
    return piecewise(other, &impl::add<T>);
  }

  Matrix operator+(const Matrix &other) const { return add(other); }

  [[nodiscard]] Matrix subtract(const Matrix &other) const
    requires(impl::Subtractible<T>)
  {
    return piecewise(other, &impl::subtract<T>);
  }

  Matrix operator-(const Matrix &other) const { return subtract(other); }

  [[nodiscard]] Matrix<T, R, 1> multiply(const Matrix<T, R, 1> &rhs) const
    requires(R == C)
  {
    Matrix<T, R, 1> output{0};
    for (size_t dim{0}; dim < R; ++dim) {
      auto basis{column(dim)};
      auto component{rhs[dim]};
      for (size_t row{0}; row < R; ++row) {
        output[row] += component * basis[row];
      }
    }
    return output;
  }

  Matrix operator*(const Matrix &rhs) const {
    Matrix output;
    size_t n{C};
    for (size_t i{0}; i < C; ++i) {
      for (size_t j{0}; j < R; ++j) {
        for (size_t k{0}; k < n; ++k) {
          output[{i, j}] += at({i, k}) * rhs[{k, j}];
        }
      }
    }
    return output;
  }
};

template <typename T, size_t D>
using Vector = Matrix<T, D, 1>;

template <typename T>
using Vec2 = Vector<T, 2>;

using Vec2f = Vec2<float>;
using Vec2i = Vec2<int>;

template <typename T>
using Vec3 = Vector<T, 3>;

using Vec3f = Vec3<float>;
using Vec3i = Vec3<int>;

template <typename T>
using Vec4 = Vector<T, 4>;

using Vec4f = Vec4<float>;
using Vec4i = Vec4<int>;
} // namespace solaris
