#ifndef _PP1_TYPE_H
#define _PP1_TYPE_H

#include <concepts>
#include <stdfloat>
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


}  // namespace pp1
#endif
