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

  Array() = delete;
  Array(AllocationType alloc_type = AllocationType::MainMemoryPacked);
  Array(std::initializer_list<T> init_list,
        AllocationType alloc_type = AllocationType::MainMemoryPacked);
  Array(const std::vector<T>& from_vec,
        AllocationType alloc_type = AllocationType::MainMemoryPacked);
  template <typename InputIterator>
  Array(InputIterator begin, InputIterator end,
        AllocationType alloc_type = AllocationType::MainMemoryPacked);

  Array(const Array& other) noexcept;
  Array(const Array&& other) noexcept;

  Array& operator=(const Array& other) noexcept;
  Array& operator=(Array&& other) noexcept;

  template <size_t RhsFirstAxis, size_t... RhsRestAxis>
  Array<T, RhsFirstAxis, RhsRestAxis...> Reshape(
      const Dimension<RhsFirstAxis, RhsRestAxis...>& dim_to_shape);

  static Array Defaults(
      AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::default_initializable<T>;

  static Array Ones(
      AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::is_arithmetic_v<T>;

  template <typename... Args>
  static Array Emplace(Args... args, AllocationType alloc_type =
                                         AllocationType::MainMemoryPacked)
    requires std::is_constructible_v<T, Args...>;

  Array operator*(const T scalar)
    requires Multipliable<T>;
  Array operator/(const T scalar)
    requires Divisable<T>;
  Array operator+(const T scalar)
    requires Addable<T>;
  Array operator-(const T scalar)
    requires Subtractable<T>;

  template <typename U>
  Array& operator*=(const U scalar)
    requires Multipliable<T>;
  template <typename U>
  Array& operator/=(const U scalar)
    requires Divisable<T>;
  template <typename U>
  Array& operator+=(const U scalar)
    requires Addable<T>;
  template <typename U>
  Array& operator-=(const U scalar)
    requires Subtractable<T>;

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
  template <typename InputIterator>
  void InitBuffer(InputIterator begin, InputIterator end,
                  AllocationType alloc_type);

  Dimension<FirstAxis, RestAxis...> dim_;
  std::shared_ptr<ArrayData<T>> data_;
};

template <typename T, size_t FirstAxis, size_t... RestAxis>
Array<T, FirstAxis, RestAxis...>::Array(AllocationType alloc_type)
    : data_(dim_, alloc_type) {}

template <typename T, size_t FirstAxis, size_t... RestAxis>
Array<T, FirstAxis, RestAxis...>::Array(std::initializer_list<T> init_list,
                                        AllocationType alloc_type) {
  static_assert(init_list.size() == dim_.Size(),
                "The number of arguments cannot form the given dimension");
  InitBuffer(init_list.begin(), init_list.end(), alloc_type);
}

template <typename T, size_t FirstAxis, size_t... RestAxis>
Array<T, FirstAxis, RestAxis...>::Array(const std::vector<T>& from_vec,
                                        AllocationType alloc_type) {
  InitBuffer(from_vec.begin(), from_vec.end(), alloc_type);
}

template <typename T, size_t FirstAxis, size_t... RestAxis>
template <typename InputIterator>
Array<T, FirstAxis, RestAxis...>::Array(InputIterator begin, InputIterator end,
                                        AllocationType alloc_type) {
  InitBuffer(begin, end, alloc_type);
}

template <typename T, size_t FirstAxis, size_t... RestAxis>
Array<T, FirstAxis, RestAxis...>::Array(const Array& other) noexcept {
  this->data_ = other->data_;
}
template <typename T, size_t FirstAxis, size_t... RestAxis>
Array<T, FirstAxis, RestAxis...>::Array(const Array&& other) noexcept
    : data_(std::move(other->data_)) {}

template <typename T, size_t FirstAxis, size_t... RestAxis>
Array<T, FirstAxis, RestAxis...>& Array<T, FirstAxis, RestAxis...>::operator=(
    const Array<T, FirstAxis, RestAxis...>& other) noexcept {
  this->data_ = other->data_;
}

template <typename T, size_t FirstAxis, size_t... RestAxis>
Array<T, FirstAxis, RestAxis...>& Array<T, FirstAxis, RestAxis...>::operator=(
    Array<T, FirstAxis, RestAxis...>&& other) noexcept {
  this->data_ = std::move(other);
}

template <typename T, size_t FirstAxis, size_t... RestAxis>
template <size_t RhsFirstAxis, size_t... RhsRestAxis>
Array<T, RhsFirstAxis, RhsRestAxis...>
Array<T, FirstAxis, RestAxis...>::Reshape(
    const Dimension<RhsFirstAxis, RhsRestAxis...>& dim_to_shape) {
  static_assert(dim_to_shape.Size() == dim_.Size(),
                "Cannot reshape with the given dimension");
  Array<T, RhsFirstAxis, RhsRestAxis...> reshaped;
  reshaped.data_ = this->data_;
  return reshaped;
}

template <typename T, size_t FirstAxis, size_t... RestAxis>
template <typename InputIterator>
void Array<T, FirstAxis, RestAxis...>::InitBuffer(InputIterator begin,
                                                  InputIterator end,
                                                  AllocationType alloc_type) {
  assert(std::distance(begin, end) == dim_.size());
  data_ = std::make_shared<ArrayData<T>>(dim_, alloc_type);
  data_->FillIn(begin, end);
}

}  // namespace pp1
#endif