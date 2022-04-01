#ifndef GENEBITS_ENGINE_MATH_MAT2X2_H
#define GENEBITS_ENGINE_MATH_MAT2X2_H

#include "genebits/engine/math/matrix_compute.h"
#include "genebits/engine/math/vec2.h"

namespace genebits::engine
{
template<typename T, size_t Rows, size_t Cols>
struct Mat;

///
/// 2x2 mathematical matrix.
///
/// The matrix is stored in column major order.
///
/// The memory layout of the matrix is: [c0r0, c0r1, c1r0, c1r1]
///
/// @tparam T The type of the matrix elements.
///
template<typename T>
struct Mat<T, 2, 2>
{
public:
  using value_type = T;

  static constexpr size_t rows = 2;
  static constexpr size_t cols = 2;

  using col_type = Vec<T, rows>;
  using row_type = Vec<T, cols>;
  using transpose_type = Mat<T, rows, cols>;

private:
  col_type data[2];

public:
  ///
  /// Default constructor.
  ///
  /// Initializes the matrix to the identity matrix.
  ///
  constexpr Mat() noexcept : data { col_type(1, 0), col_type(0, 1) } {} // Identity

  ///
  /// Scalar constructor.
  ///
  /// Initializes the matrix to the scaled identity matrix.
  ///
  /// @param[in] scalar The scalar to scale the identity matrix by.
  ///
  constexpr Mat(T scalar) noexcept : data { col_type(scalar, 0), col_type(0, scalar) } {} // Identity

  ///
  /// Parametric constructor.
  ///
  /// @param[in] x0 The x component of the first column.
  /// @param[in] y0 The y component of the first column.
  /// @param[in] x1 The x component of the second column.
  /// @param[in] y1 The y component of the second column.
  ///
  constexpr Mat(T x0, T y0, T x1, T y1) noexcept : data { col_type(x0, y0), col_type(x1, y1) } {}

  ///
  /// Column constructor.
  ///
  /// @param a The first column.
  /// @param b The second column.
  ///
  constexpr Mat(const col_type& a, const col_type& b) noexcept : data { a, b } {}

  ///
  /// Copy constructor.
  ///
  /// @param[in] other The matrix to copy.
  ///
  constexpr Mat(const Mat& other) noexcept = default;

  ///
  /// Copy constructor.
  ///
  /// @tparam U The type of the other matrix.
  ///
  /// @param[in] other The matrix to copy.
  ///
  template<typename U>
  constexpr explicit Mat(const Mat<U, rows, cols>& other) noexcept : data { col_type(other[0]), col_type(other[1]) }
  {}

  ///
  /// Add assignment operator.
  ///
  /// @param[in] scalar The scalar to add.
  ///
  /// @return A reference to the matrix.
  ///
  constexpr Mat& operator+=(T scalar) noexcept
  {
    data[0] += scalar;
    data[1] += scalar;
    return *this;
  }

  ///
  /// Add assignment operator.
  ///
  /// @param[in] other The matrix to add.
  ///
  /// @return A reference to the matrix.
  ///
  constexpr Mat& operator+=(const Mat& other) noexcept
  {
    data[0] += other[0];
    data[1] += other[1];
    return *this;
  }

  ///
  /// Subtract assignment operator.
  ///
  /// @param[in] scalar The scalar to subtract.
  ///
  /// @return A reference to the matrix.
  ///
  constexpr Mat& operator-=(T scalar) noexcept
  {
    data[0] -= scalar;
    data[1] -= scalar;
    return *this;
  }

  ///
  /// Subtract assignment operator.
  ///
  /// @param[in] other The matrix to subtract.
  ///
  /// @return A reference to the matrix.
  ///
  constexpr Mat& operator-=(const Mat& other) noexcept
  {
    data[0] -= other[0];
    data[1] -= other[1];
    return *this;
  }

  ///
  /// Multiply assignment operator.
  ///
  /// @param[in] scalar The scalar to multiply.
  ///
  /// @return A reference to the matrix.
  ///
  constexpr Mat& operator*=(T scalar) noexcept
  {
    data[0] *= scalar;
    data[1] *= scalar;
    return *this;
  }

  ///
  /// Multiply assignment operator.
  ///
  /// @param[in] other The matrix to multiply.
  ///
  /// @return A reference to the matrix.
  ///
  constexpr Mat& operator*=(const Mat& other) noexcept
  {
    return (*this = Product(*this, other));
  }

  ///
  /// Divide assignment operator.
  ///
  /// @param[in] scalar The scalar to divide.
  ///
  /// @return A reference to the matrix.
  ///
  constexpr Mat& operator/=(T scalar) noexcept
  {
    data[0] /= scalar;
    data[1] /= scalar;
    return *this;
  }

  ///
  /// Pre-increment operator.
  ///
  /// @return A reference to the matrix.
  ///
  constexpr Mat& operator++() noexcept
  {
    ++data[0];
    ++data[1];
    return *this;
  }

  ///
  /// Post-increment operator.
  ///
  /// @return A copy of the matrix.
  ///
  constexpr const Mat operator++(int) noexcept
  {
    Mat tmp(*this);
    return ++(*this), tmp;
  }

  ///
  /// Pre-decrement operator.
  ///
  /// @return A reference to the matrix.
  ///
  constexpr Mat& operator--() noexcept
  {
    --data[0];
    --data[1];
    return *this;
  }

  ///
  /// Post-decrement operator.
  ///
  /// @return A copy of the matrix.
  ///
  constexpr const Mat operator--(int) noexcept
  {
    Mat tmp(*this);
    return --(*this), tmp;
  }

  ///
  /// Subscript operator.
  ///
  /// Accesses the column at the specified index.
  ///
  /// @param[in] index The index corresponding to the column.
  ///
  /// @return A reference to the column.
  ///
  [[nodiscard]] constexpr const col_type& operator[](size_t index) const noexcept
  {
    ASSERT(index < cols, "Matrix column out of range");
    return data[index];
  }

  ///
  /// Subscript operator.
  ///
  /// Accesses the column at the specified index.
  ///
  /// @param[in] index The index corresponding to the column.
  ///
  /// @return A reference to the column.
  ///
  [[nodiscard]] constexpr col_type& operator[](size_t index) noexcept
  {
    ASSERT(index < cols, "Matrix column out of range");
    return data[index];
  }

  ///
  /// Add operator.
  ///
  /// @param[in] scalar The scalar to add.
  ///
  /// @return The result of the addition.
  ///
  [[nodiscard]] friend constexpr Mat operator+(const Mat& mat, T scalar) noexcept
  {
    return Mat { mat[0] + scalar, mat[1] + scalar };
  }

  ///
  /// Add operator.
  ///
  /// @param[in] lhs Left-hand side matrix.
  /// @param[in] rhs Right-hand side matrix.
  ///
  /// @return The result of the addition.
  ///
  [[nodiscard]] friend constexpr Mat operator+(const Mat& lhs, const Mat& rhs) noexcept
  {
    return Mat { lhs[0] + rhs[0], lhs[1] + rhs[1] };
  }

  ///
  /// Subtract operator.
  ///
  /// @param[in] scalar The scalar to add.
  ///
  /// @return The result of the subtraction.
  ///
  [[nodiscard]] friend constexpr Mat operator-(const Mat& mat, T scalar) noexcept
  {
    return Mat { mat[0] - scalar, mat[1] - scalar };
  }

  ///
  /// Subtract operator.
  ///
  /// @param[in] lhs Left-hand side matrix.
  /// @param[in] rhs Right-hand side matrix.
  ///
  /// @return The result of the subtraction.
  ///
  [[nodiscard]] friend constexpr Mat operator-(const Mat& lhs, const Mat& rhs) noexcept
  {
    return Mat { lhs[0] - rhs[0], lhs[1] - rhs[1] };
  }

  ///
  /// Multiplication operator.
  ///
  /// @param[in] scalar The scalar to multiply.
  ///
  /// @return The product of the matrix and the scalar.
  ///
  [[nodiscard]] friend constexpr Mat operator*(const Mat& mat, T scalar) noexcept
  {
    return Mat { mat[0] * scalar, mat[1] * scalar };
  }

  ///
  /// Multiplication operator.
  ///
  /// Performs matrix-vector multiplication.
  ///
  /// @param[in] mat Matrix to multiply vector by.
  /// @param[in] vec Vector to transform.
  ///
  /// @return The transformed vector.
  ///
  [[nodiscard]] friend constexpr Mat::col_type operator*(const Mat& mat, const Mat::row_type& vec) noexcept
  {
    return Mat::col_type { mat[0][0] * vec.x + mat[1][0] * vec.y, mat[0][1] * vec.x + mat[1][1] * vec.y };
  }

  ///
  /// Multiplication operator.
  ///
  /// Performs vector-matrix multiplication.
  ///
  /// @param[in] mat Matrix to multiply.
  /// @param[in] vec Vector to multiply vector with.
  ///
  /// @return The transformed vector.
  ///
  [[nodiscard]] friend constexpr Mat::row_type operator*(const Mat::col_type& vec, const Mat& mat) noexcept
  {
    return Mat::row_type { mat[0][0] * vec.x + mat[0][1] * vec.y, mat[1][0] * vec.x + mat[1][1] * vec.y };
  }

  ///
  /// Multiplication operator.
  ///
  /// Performs matrix-matrix multiplication.
  ///
  /// @param[in] lhs Left-hand side matrix.
  /// @param[in] rhs Right-hand side matrix.
  ///
  /// @return The product of the matrices.
  ///
  [[nodiscard]] friend constexpr Mat operator*(const Mat& lhs, const Mat& rhs) noexcept
  {
    return Mat { //
      lhs[0][0] * rhs[0][0] + lhs[1][0] * rhs[0][1], //
      lhs[0][1] * rhs[0][0] + lhs[1][1] * rhs[0][1], //
      lhs[0][0] * rhs[1][0] + lhs[1][0] * rhs[1][1], //
      lhs[0][1] * rhs[1][0] + lhs[1][1] * rhs[1][1]
    };
  }

  ///
  /// Division operator.
  ///
  /// @param[in] scalar The scalar to divide.
  ///
  /// @return The quotient of the matrix and the scalar.
  ///
  [[nodiscard]] friend constexpr Mat operator/(const Mat& mat, T scalar) noexcept
  {
    return Mat { mat[0] / scalar, mat[1] / scalar };
  }

  ///
  /// Equality operator.
  ///
  /// @param[in] other The matrix to compare.
  ///
  /// @return True if the matrices are equal, false otherwise.
  ///
  [[nodiscard]] friend constexpr bool operator==(const Mat& lhs, const Mat& rhs) noexcept
  {
    return (lhs[0] == rhs[0]) && (lhs[1] == rhs[1]);
  }

  ///
  /// Inequality operator.
  ///
  /// @param[in] other The matrix to compare.
  ///
  /// @return True if the matrices are not equal, false otherwise.
  ///
  [[nodiscard]] friend constexpr bool operator!=(const Mat& lhs, const Mat& rhs) noexcept
  {
    return !(lhs == rhs);
  }
};

using bool2x2 = Mat<bool, 2, 2>;
using int2x2 = Mat<int, 2, 2>;
using uint2x2 = Mat<unsigned int, 2, 2>;
using float2x2 = Mat<float, 2, 2>;
using double2x2 = Mat<double, 2, 2>;
} // namespace genebits::engine

namespace std
{
template<typename T>
struct hash<genebits::engine::Mat<T, 2, 2>>
{
  constexpr size_t operator()(const genebits::engine::Mat<T, 2, 2>& mat) const noexcept
  {
    return std::hash<T>()(mat[0]) ^ std::hash<T>(mat[1]);
  }
};
} // namespace std

#endif // GENEBITS_VEC_H
