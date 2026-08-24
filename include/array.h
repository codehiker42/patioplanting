#ifndef _PP1_ARRAY_H_
#define _PP1_ARRAY_H_

#include <initializer_list>
#include <memory>
#include <vector>

#include "arraydata.h"
#include "dimension.h"
#include "pp1_type.h"

namespace pp1 {

template <typename T = Types::DefaultRealT>
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
  Array(std::initializer_list<T> init_list);
  Array(const std::vector<T>& from_vec);
  Array(const Array& other);
  Array(const Array&& other);

  Array& operator=(const A<T>& other);

  static Array<T> Zeros(const Dimension& dim);
  static Array<T> Ones(const Dimension& dim);
  static Array<T> Empty(const Dimension& dim);
  template <typename RandomGen>
  static Array<T> Empty(const Dimension& dim, RandomGen&& gen);

  const Dimension& Dim() const;

  Array operator*(const T scalar);
  Array operator-(const T scalar);
  Array operator+(const T scalar);
  Array operator/(const T scalar);

  template<typename U>
  Array& operator*=(const U scalar);
  template<typename U>
  Array& operator-=(const U scalar);
  template<typename U>
  Array& operator+=(const U scalar);
  template<typename U>
  Array& operator/=(const U scalar);

  template<typename U>
  Array<std::common_type_t<T, U>>& operator*=(const Array<U>& rhs);
  template<typename U>
  Array<std::common_type_t<T, U>>& operator/=(const Array<U>& rhs);
  template<typename U>
  Array<std::common_type_t<T, U>>& operator+=(const Array<U>& rhs);
  template<typename U>
  Array<std::common_type_t<T, U>>& operator-=(const Array<U>& rhs);

 private:
  Dimension dim_;
  std::shared_ptr<ArrayData<T>> data_;
};

}  // namespace pp1
#endif