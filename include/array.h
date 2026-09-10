#ifndef _PP1_ARRAY_H_
#define _PP1_ARRAY_H_

#include <initializer_list>
#include <memory>
#include <vector>

#include "arraydata.h"
#include "domain.h"
#include "pp1_type.h"
#include "pp1_utility.h"

namespace pp1 {

template <typename T = Types::DefaultRealT, size_t FirstDimension = 1,
          size_t... RestDimensions>
class Array {
 public:
  using reference = T&;
  using const_reference = std::add_const_t<T>&;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using const_pointer = std::add_const_t<T>*;
  using domain = Domain<FirstDimension, RestDimensions...>;

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

  Array(T&& t,
        const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::copyable<T>;

  template <typename FirstTuple, typename... RestTuples>
  Array(const AllocationType alloc_type, FirstTuple&& first_elem,
        RestTuples&&... rest_elems)
    requires TupleForT<T, FirstTuple> && (TupleForT<T, RestTuples> && ...);

  template <typename U, typename V>
  Array(const MDInitList<T, U, V, FirstDimension, RestDimensions...>&
            md_init_list,
        const AllocationType alloc_type = AllocationType::MainMemoryPacked);

  Array(const Seq<T, FirstDimension, RestDimensions...>& seq,
        const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::semiregular<T> && std::equality_comparable<T>;

  template <size_t RhsFirstDimension, size_t... RhsRestDimensions>
  Array<T, RhsFirstDimension, RhsRestDimensions...> Reshape(
      const Domain<RhsFirstDimension, RhsRestDimensions...>& dim_to_shape);

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
  Array<std::common_type_t<T, U>, FirstDimension, RestDimensions...>&
  operator*=(const Array<U, FirstDimension, RestDimensions...>& rhs);
  template <typename U>
  Array<std::common_type_t<T, U>, FirstDimension, RestDimensions...>&
  operator/=(const Array<U, FirstDimension, RestDimensions...>& rhs);
  template <typename U>
  Array<std::common_type_t<T, U>, FirstDimension, RestDimensions...>&
  operator+=(const Array<U, FirstDimension, RestDimensions...>& rhs);
  template <typename U>
  Array<std::common_type_t<T, U>, FirstDimension, RestDimensions...>&
  operator-=(const Array<U, FirstDimension, RestDimensions...>& rhs);

 private:
  template <std::input_iterator IT>
  void InitBuffer(IT begin, IT end, AllocationType alloc_type);

  std::shared_ptr<ArrayData<T>> data_;
};

template <typename T, size_t FirstDimension, size_t... RestDimensions>
Array<T, FirstDimension, RestDimensions...>::Array(
    const Array& other) noexcept {
  this->data_ = other->data_;
}
template <typename T, size_t FirstDimension, size_t... RestDimensions>
Array<T, FirstDimension, RestDimensions...>::Array(const Array&& other) noexcept
    : data_(std::move(other->data_)) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
Array<T, FirstDimension, RestDimensions...>&
Array<T, FirstDimension, RestDimensions...>::operator=(
    const Array<T, FirstDimension, RestDimensions...>& other) noexcept {
  this->data_ = other->data_;
}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
Array<T, FirstDimension, RestDimensions...>&
Array<T, FirstDimension, RestDimensions...>::operator=(
    Array<T, FirstDimension, RestDimensions...>&& other) noexcept {
  this->data_ = std::move(other);
}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
Array<T, FirstDimension, RestDimensions...>::Array(
    const AllocationType alloc_type)
  requires std::default_initializable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, domain())) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <std::input_iterator IT>
Array<T, FirstDimension, RestDimensions...>::Array(
    IT begin, IT end, const AllocationType alloc_type)
  requires std::copyable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, domain(), begin, end)) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
Array<T, FirstDimension, RestDimensions...>::Array(
    std::initializer_list<T> init_list, const AllocationType alloc_type)
  requires std::copyable<T>
    : data_(std::make_shared<ArrayData<T>>(
          alloc_type, domain(), init_list.begin(), init_list.end())) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
Array<T, FirstDimension, RestDimensions...>::Array(
    const std::vector<T>& from_vec, const AllocationType alloc_type)
  requires std::copyable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, domain(),
                                           from_vec.begin(), from_vec.end())) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
Array<T, FirstDimension, RestDimensions...>::Array(
    T&& t, const AllocationType alloc_type)
  requires std::copyable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, domain(),
                                           std::forward<T>(t))) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <typename FirstTuple, typename... RestTuples>
Array<T, FirstDimension, RestDimensions...>::Array(
    const AllocationType alloc_type, FirstTuple&& first_elem,
    RestTuples&&... rest_elems)
  requires TupleForT<T, FirstTuple> && (TupleForT<T, RestTuples> && ...)
    : data_(std::make_shared<ArrayData<T>>(
          alloc_type, domain(), std::forward<FirstTuple>(first_elem),
          std::forward<RestTuples>(rest_elems)...)) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <typename U, typename V>
Array<T, FirstDimension, RestDimensions...>::Array(
    const MDInitList<T, U, V, FirstDimension, RestDimensions...>& md_init_list,
    const AllocationType alloc_type)
    : data_(std::make_shared<ArrayData<T>>(
          alloc_type, domain(), md_init_list.begin(), md_init_list.end()))

{}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
Array<T, FirstDimension, RestDimensions...>::Array(
    const Seq<T, FirstDimension, RestDimensions...>& seq,
    const AllocationType alloc_type)
  requires std::semiregular<T> && std::equality_comparable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, seq)) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <size_t RhsFirstDimension, size_t... RhsRestDimensions>
Array<T, RhsFirstDimension, RhsRestDimensions...>
Array<T, FirstDimension, RestDimensions...>::Reshape(
    const Domain<RhsFirstDimension, RhsRestDimensions...>& domain_to_shape) {
  static_assert(domain_to_shape.Size() == domain().Size(),
                "Cannot reshape with the given dimension");
  Array<T, RhsFirstDimension, RhsRestDimensions...> reshaped;
  reshaped.data_ = this->data_;
  return reshaped;
}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <std::input_iterator IT>
void Array<T, FirstDimension, RestDimensions...>::InitBuffer(
    IT begin, IT end, AllocationType alloc_type) {
  assert(std::distance(begin, end) == domain().size());
  data_ = std::make_shared<ArrayData<T>>(domain(), alloc_type);
  data_->FillIn(begin, end);
}

}  // namespace pp1
#endif