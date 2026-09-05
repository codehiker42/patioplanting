#ifndef _PP1_TYPE_H
#define _PP1_TYPE_H

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
struct is_tuple_for_typet < T, std::tuple<Args...>>
    : std::bool_constant<std::constructible_from<T, Args...>> {};

template <class T, class Tuple>
concept TupleForT = is_tuple_for_typet<T, std::remove_cvref_t<Tuple>>::value;

}  // namespace pp1
#endif
