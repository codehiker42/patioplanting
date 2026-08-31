#ifndef _PP1_ARRAY_H_
#define _PP1_ARRAY_H_

#include <initializer_list>
#include <memory>
#include <vector>

#include "arraydata.h"
#include "dimension.h"
#include "pp1_type.h"

namespace pp1 {

template <typename T = Types::DefaultRealT, size_t FirstAxis = 1,
          size_t... RestAxis>
class Array {
 public:
  using reference = T&;
  using const_reference = std::add_const_t<T>&;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using const_pointer = std::add_const_t<T>*;

  Array() = default;
  Array(std::initializer_list<T>& init_list);
  Array(const std::vector<T>& from_vec);
  Array(const Array& other);
  Array(const Array&& other);

  Array& operator=(const Array& other);

  template <size_t RhsFirstAxis, size_t... RhsRestAxis>
  Array<T, RhsFirstAxis, RhsRestAxis...> Reshape(
      Dimension<RhsFirstAxis, RhsRestAxis...>);

  static Array Zeros();

  static Array Ones();

  static Array Empty();

  template <typename RandomGen>
  static Array Empty(RandomGen&& gen);

  Array operator*(const T scalar);
  Array operator-(const T scalar);
  Array operator+(const T scalar);
  Array operator/(const T scalar);

  template <typename U>
  Array& operator*=(const U scalar);
  template <typename U>
  Array& operator-=(const U scalar);
  template <typename U>
  Array& operator+=(const U scalar);
  template <typename U>
  Array& operator/=(const U scalar);

  template <typename U>
  Array<std::common_type_t<T, U>, FirstAxis, RestAxis...>& operator*=(
      const Array<U, FirstAxis, RestAxis...>& rhs);
  template <typename U>
  Array<std::common_type_t<T, U>, FirstAxis, RestAxis...>& operator/=(
      const Array<U, FirstAxis, RestAxis...>& rhs);
  template <typename U>
  Array<std::common_type_t<T, U>, FirstAxis, RestAxis...>& operator+=(
      const Array<U, FirstAxis, RestAxis...>& rhs);
  template <typename U>
  Array<std::common_type_t<T, U>, FirstAxis, RestAxis...>& operator-=(
      const Array<U, FirstAxis, RestAxis...>& rhs);

 private:
  Dimension<FirstAxis, RestAxis...> dim_;
  std::shared_ptr<ArrayData<T>> data_;
};

}  // namespace pp1
#endif