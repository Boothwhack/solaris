#include <catch2/catch_test_macros.hpp>
#include <solaris/math/matrix.hpp>

using solaris::Matrix;
using solaris::Vector;

TEST_CASE("Matrix access", "[math][Matrix]") {
  // clang-format off
  Matrix<int, 2, 2> matrix{
    0, 1,
    2, 3,
  };
  // clang-format on

  REQUIRE(matrix[{0, 0}] == 0);
  REQUIRE(matrix[{1, 0}] == 1);
  REQUIRE(matrix[{0, 1}] == 2);
  REQUIRE(matrix[{1, 1}] == 3);
}

TEST_CASE("Vector access", "[math][Vector]") {
  Vector<float, 2> vector;
  vector.x() = 1.0;
  vector.y() = 2.0;

  REQUIRE(vector.x() == 1.0);
  REQUIRE(vector.y() == 2.0);
}

TEST_CASE("Matrix equality", "[math][Matrix]") {
  Matrix<int, 2, 2> matrixA{1, 2, 3, 4};
  Matrix<int, 2, 2> matrixB{1, 2, 3, 4};
  Matrix<int, 2, 2> matrixC{1, 3, 2, 4};

  REQUIRE(matrixA == matrixB);
  REQUIRE(matrixB == matrixA);
  REQUIRE(matrixA != matrixC);
  REQUIRE(matrixC != matrixA);
}

TEST_CASE("Matrix sub-matrix", "[math][Matrix]") {
  {
    // clang-format off
    Matrix<int, 4, 4> matrix{
      00, 10, 20, 30,
      01, 11, 21, 31,
      02, 12, 22, 32,
      03, 13, 23, 33,
    };
    // clang-format on

    auto sub{matrix.subMatrix<2, 2>({1, 0})};
    REQUIRE(sub == Matrix<int, 2, 2>(10, 20, 11, 21));
  }
  {
    // clang-format off
    Matrix<int, 2, 2> matrix{
      1, 0,
      0, 1,
    };
    // clang-format on

    REQUIRE(matrix.subMatrix<2, 1>({0, 0}) == Matrix<int, 2, 1>(1, 0));
    REQUIRE(matrix.subMatrix<2, 1>({1, 0}) == Matrix<int, 2, 1>(0, 1));
  }
}

TEST_CASE("Matrix add", "[math][Matrix]") {
  Matrix<int, 2, 2> matrixA{1, 2, 3, 4};
  Matrix<int, 2, 2> matrixB{1, 3, 2, 4};

  auto result{matrixA + matrixB};

  REQUIRE(result[{0, 0}] == 2);
  REQUIRE(result[{1, 0}] == 5);
  REQUIRE(result[{0, 1}] == 5);
  REQUIRE(result[{1, 1}] == 8);
}

TEST_CASE("Matrix subtract", "[math][Matrix]") {
  Matrix<int, 2, 2> matrixA{1, 2, 3, 4};
  Matrix<int, 2, 2> matrixB{1, 3, 2, 4};

  auto result{matrixA - matrixB};

  REQUIRE(result[{0, 0}] == 0);
  REQUIRE(result[{1, 0}] == -1);
  REQUIRE(result[{0, 1}] == 1);
  REQUIRE(result[{1, 1}] == 0);
}

TEST_CASE("Matrix identity transformation", "[math][Matrix][Vector]") {
  {
    // clang-format off
    Matrix<float, 2, 2> identity{
      1, 0,
      0, 1,
    };
    Vector<float, 2> vector{
      2,
      1,
    };
    // clang-format on

    auto transformed{identity.multiply(vector)};

    REQUIRE(transformed == Vector<float, 2>(2, 1));
  }
}

TEST_CASE("Matrix linear transformation", "[math][Matrix][Vector]") {
  // clang-format off
  Matrix<float, 2, 2> rotation{ // 90-degree counter-clockwise rotation
    0, -1,
    1, 0,
  };
  Vector<float, 2> vector{
    2,
    1,
  };
  // clang-format on

  auto transformed{rotation * vector};

  REQUIRE(transformed == Vector<float, 2>(-1, 2));
}
