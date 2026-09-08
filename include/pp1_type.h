#ifndef _PP1_TYPE_H
#define _PP1_TYPE_H

#include <array>
#include <concepts>
#include <stdfloat>
#include <tuple>
#include <type_traits>

namespace pp1 {

struct Types {
  using DefaultRealT = ::std::float32_t;
};

template <typename T>
concept Multipliable = requires(const T& a, const T& b) {
  { a * b } -> std::same_as<T>;
};

template <typename T>
concept Divisable = requires(const T& a, const T& b) {
  { a / b } -> std::same_as<T>;
};

template <typename T>
concept Addable = requires(const T& a, const T& b) {
  { a + b } -> std::same_as<T>;
};

template <typename T>
concept Subtractable = requires(const T& a, const T& b) {
  { a - b } -> std::same_as<T>;
};

template <typename T, typename TupleOrNot>
struct is_tuple_for_typet : std::false_type {};

template <typename T, typename... Args>
struct is_tuple_for_typet<T, std::tuple<Args...>>
    : std::bool_constant<std::constructible_from<T, Args...>> {};

template <class T, class Tuple>
concept TupleForT = is_tuple_for_typet<T, std::remove_cvref_t<Tuple>>::value;

template <class T>
struct IsStdArray : std::false_type {};

template <class T, std::size_t N>
struct IsStdArray<std::array<T, N>> : std::true_type {};

template <typename T, typename OuterArr, typename V, size_t Rank>
class MDInitListIterator {
 public:
  using difference_type = std::ptrdiff_t;
  using value_type = T;
  using iterator_type = T;

  using index_arr_t = std::array<size_t, Rank>;

  MDInitListIterator(const OuterArr& md_arr, const index_arr_t& n_dims,
                     const size_t index, const size_t limit);

  MDInitListIterator(const MDInitListIterator& rhs);

  MDInitListIterator(MDInitListIterator&& rhs);

  MDInitListIterator& operator=(const MDInitListIterator& rhs);

  MDInitListIterator& operator=(MDInitListIterator&& rhs);

  constexpr value_type operator*() const noexcept;

  constexpr MDInitListIterator& operator++() noexcept;
  constexpr MDInitListIterator operator++(int) noexcept;

  constexpr bool operator==(const MDInitListIterator& rhs) const noexcept;

  template <typename Arr>
    requires IsStdArray<std::remove_cvref_t<Arr>>::value &&
             (!IsStdArray<std::remove_cvref_t<typename Arr::value_type>>::value)
  constexpr decltype(auto) get(const Arr& arr, const index_arr_t& indices,
                               size_t level) const noexcept;

  template <typename Arr>
    requires IsStdArray<std::remove_cvref_t<Arr>>::value &&
             IsStdArray<std::remove_cvref_t<typename Arr::value_type>>::value
  constexpr decltype(auto) get(const Arr& arr, const index_arr_t& indices,
                               size_t level) const noexcept;

 private:
  const OuterArr& md_arr_;
  const std::array<size_t, Rank>& n_dims_;
  const size_t limit_;
  size_t index_{0};
};

template <typename T, typename OuterArr, typename V, size_t Rank>
MDInitListIterator<T, OuterArr, V, Rank>::MDInitListIterator(
    const OuterArr& md_arr, const index_arr_t& n_dims, const size_t index,
    const size_t limit)
    : md_arr_(md_arr), n_dims_(n_dims), index_(index), limit_(limit) {}

template <typename T, typename OuterArr, typename V, size_t Rank>
MDInitListIterator<T, OuterArr, V, Rank>::MDInitListIterator(
    const MDInitListIterator& rhs)
    : md_arr_(rhs.md_arr_),
      n_dims_(rhs.n_dims_),
      limit_(rhs.limit_),
      index_(rhs.index_) {}

template <typename T, typename OuterArr, typename V, size_t Rank>
MDInitListIterator<T, OuterArr, V, Rank>::MDInitListIterator(
    MDInitListIterator&& rhs)
    : md_arr_(rhs.md_arr_),
      n_dims_(rhs.n_dims_),
      limit_(rhs.limit_),
      index_(rhs.index_) {}


template <typename T, typename OuterArr, typename V, size_t Rank>
MDInitListIterator<T, OuterArr, V, Rank>&
MDInitListIterator<T, OuterArr, V, Rank>::operator=(const MDInitListIterator& rhs) {
  index_ = rhs.index_;
  return *this;
}

template <typename T, typename OuterArr, typename V, size_t Rank>
MDInitListIterator<T, OuterArr, V, Rank>&
MDInitListIterator<T, OuterArr, V, Rank>::operator=(MDInitListIterator&& rhs) {
  index_ = rhs.index_;
  return *this;
}

template <typename T, typename OuterArr, typename V, size_t Rank>
template <typename Arr>
  requires IsStdArray<std::remove_cvref_t<Arr>>::value &&
           (!IsStdArray<std::remove_cvref_t<typename Arr::value_type>>::value)

constexpr decltype(auto) MDInitListIterator<T, OuterArr, V, Rank>::get(
    const Arr& arr, const index_arr_t& indices, size_t level) const noexcept {
  return arr.at(indices.at(level));
}

template <typename T, typename OuterArr, typename V, size_t Rank>
template <typename Arr>
  requires IsStdArray<std::remove_cvref_t<Arr>>::value &&
           IsStdArray<std::remove_cvref_t<typename Arr::value_type>>::value
constexpr decltype(auto) MDInitListIterator<T, OuterArr, V, Rank>::get(
    const Arr& arr, const index_arr_t& indices, size_t level) const noexcept {
  return get(arr.at(indices.at(level)), indices, level + 1);
}

template <typename T, typename OuterArr, typename V, size_t Rank>
constexpr MDInitListIterator<T, OuterArr, V, Rank>::value_type
MDInitListIterator<T, OuterArr, V, Rank>::operator*() const noexcept {
  // undefined behaviour for *end()
  index_arr_t indices{0};
  size_t index_copy = index_;

  for (size_t i = 0; i < indices.size() && index_copy != 0; ++i) {
    indices[i] = index_copy / n_dims_.at(i);
    index_copy %= n_dims_.at(i);
  }

  return static_cast<value_type>(get(md_arr_, indices, 0));
}

template <typename T, typename OuterArr, typename V, size_t Rank>
constexpr MDInitListIterator<T, OuterArr, V, Rank>&
MDInitListIterator<T, OuterArr, V, Rank>::operator++() noexcept {
  if (index_ < limit_) {
    ++index_;
  }
  return *this;
}

template <typename T, typename OuterArr, typename V, size_t Rank>
constexpr MDInitListIterator<T, OuterArr, V, Rank>
MDInitListIterator<T, OuterArr, V, Rank>::operator++(int) noexcept {
  auto ret = *this;
  this->operator++();
  return ret;
}

template <typename T, typename OuterArr, typename V, size_t Rank>
constexpr bool MDInitListIterator<T, OuterArr, V, Rank>::operator==(
    const MDInitListIterator& rhs) const noexcept {
  return index_ == rhs.index_;
}

template <typename T, typename U, typename V, size_t FirstDimension,
          size_t... RestDimensions>
class MDInitList {
 public:
  using value_type = T;
  using first_elem_type = U;
  using elem_type = V;
  using domain = Domain<FirstDimension, RestDimensions...>;
  using outer_array_t = std::array<U, FirstDimension>;
  using iterator = MDInitListIterator<T, outer_array_t, V, domain::Rank()>;

  MDInitList(outer_array_t md_arr);

  iterator begin() const noexcept;
  iterator end() const noexcept;

 private:
  const outer_array_t md_arr_;
  std::array<size_t, domain::Rank()> n_dims_{RestDimensions...,
                                             static_cast<size_t>(1)};
};

template <typename T, typename U, typename V, size_t FirstDimension,
          size_t... RestDimensions>
MDInitList<T, U, V, FirstDimension, RestDimensions...>::MDInitList(
    outer_array_t md_arr)
    : md_arr_(std::move(md_arr)) {
  for (int i = std::ssize(n_dims_) - 3; i >= 0; --i) {
    n_dims_[i] *= n_dims_.at(i + 1);
  }
}

template <typename T, typename U, typename V, size_t FirstDimension,
          size_t... RestDimensions>

MDInitList<T, U, V, FirstDimension, RestDimensions...>::iterator
MDInitList<T, U, V, FirstDimension, RestDimensions...>::begin() const noexcept {
  return iterator(md_arr_, n_dims_, 0, domain::Size());
}

template <typename T, typename U, typename V, size_t FirstDimension,
          size_t... RestDimensions>
MDInitList<T, U, V, FirstDimension, RestDimensions...>::iterator
MDInitList<T, U, V, FirstDimension, RestDimensions...>::end() const noexcept {
  return iterator(md_arr_, n_dims_, domain::Size(), domain::Size());
}

template <typename, typename, typename, size_t, size_t...>
struct MDInitListHelper {};

template <typename T, typename U, typename V, size_t N, size_t FirstDimension,
          size_t... RestDimensions>
  requires std::is_convertible_v<V, T> &&
           (!IsStdArray<std::remove_cvref_t<V>>::value)
struct MDInitListHelper<T, U, std::array<V, N>, FirstDimension,
                        RestDimensions...> {
  using mdlist_type = MDInitList<T, U, V, FirstDimension, RestDimensions..., N>;
};

template <typename T, typename U, typename V, size_t N, size_t FirstDimension,
          size_t... RestDimensions>
  requires IsStdArray<std::remove_cvref_t<V>>::value
struct MDInitListHelper<T, U, std::array<V, N>, FirstDimension,
                        RestDimensions...> {
  using mdlist_type =
      MDInitListHelper<T, U, V, FirstDimension, RestDimensions..., N>;
};

}  // namespace pp1
#endif
