#ifndef _PP1_ARRAY_H_
#define _PP1_ARRAY_H_

#include <initializer_list>
#include <memory>
#include <vector>

#include "arraydata.h"
#include "dimension.h"
#include "pp1_type.h"
#include "pp1_utility.h"

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

  Array(const Array& other) noexcept;
  Array(const Array&& other) noexcept;

  Array& operator=(const Array& other) noexcept;
  Array& operator=(Array&& other) noexcept;

  Array(const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::default_initializable<T>;

  template <std::input_iterator IT>
  Array(IT begin, IT end,
        const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::copyable<T>;

  Array(std::initializer_list<T> init_list,
        const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::copyable<T>;

  Array(const std::vector<T>& from_vec,
        const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::copyable<T>;

  Array(const T& t,
        const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::copyable<T>;

  template <typename... Args>
  Array(std::tuple<Args...>&& arg_tuple,
        const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::is_constructible_v<T, Args...>;

  template <size_t RhsFirstAxis, size_t... RhsRestAxis>
  Array<T, RhsFirstAxis, RhsRestAxis...> Reshape(
      const Dimension<RhsFirstAxis, RhsRestAxis...>& dim_to_shape);

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
  template <std::input_iterator IT>
  void InitBuffer(IT begin, IT end, AllocationType alloc_type);

  Dimension<FirstAxis, RestAxis...> dim_;
  std::shared_ptr<ArrayData<T>> data_;
};

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
Array<T, FirstAxis, RestAxis...>::Array(const AllocationType alloc_type)
  requires std::default_initializable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, dim_)) {}

template <typename T, size_t FirstAxis, size_t... RestAxis>
template <std::input_iterator IT>
Array<T, FirstAxis, RestAxis...>::Array(IT begin, IT end,
                                        const AllocationType alloc_type)
  requires std::copyable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, dim_, begin, end)) {}

template <typename T, size_t FirstAxis, size_t... RestAxis>
Array<T, FirstAxis, RestAxis...>::Array(std::initializer_list<T> init_list,
                                        const AllocationType alloc_type)
  requires std::copyable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, dim_, init_list.begin(),
                                           init_list.end())) {}

template <typename T, size_t FirstAxis, size_t... RestAxis>
Array<T, FirstAxis, RestAxis...>::Array(const std::vector<T>& from_vec,
                                        const AllocationType alloc_type)
  requires std::copyable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, dim_, from_vec.begin(),
                                           from_vec.end())) {}

template <typename T, size_t FirstAxis, size_t... RestAxis>
Array<T, FirstAxis, RestAxis...>::Array(const T& t,
                                        const AllocationType alloc_type)
  requires std::copyable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, dim_, t)) {}

template <typename T, size_t FirstAxis, size_t... RestAxis>
template <typename... Args>
Array<T, FirstAxis, RestAxis...>::Array(std::tuple<Args...>&& arg_tuple,
                                        const AllocationType alloc_type)
  requires std::is_constructible_v<T, Args...>
    : data_(std::apply(
          [&](Args&&... args) {
            return std::make_shared<ArrayData<T>>(alloc_type, dim_, args...);
          },
          arg_tuple)

      ) {}

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
template <std::input_iterator IT>
void Array<T, FirstAxis, RestAxis...>::InitBuffer(IT begin, IT end,
                                                  AllocationType alloc_type) {
  assert(std::distance(begin, end) == dim_.size());
  data_ = std::make_shared<ArrayData<T>>(dim_, alloc_type);
  data_->FillIn(begin, end);
}

}  // namespace pp1
#endif