#ifndef _PP1_UTILITY_H_
#define _PP1_UTILITY_H_

#include <tuple>

namespace pp1 {

template <typename... Args>
inline constexpr auto Arguments(Args&&... args) {
  return std::forward_as_tuple(args...);
}

template <class... T>
inline constexpr auto Arr(T&&... values) {
  return std::array{std::forward<T>(values)...};
}

template <typename T, typename U, size_t N>
  requires std::is_convertible_v<U, T> && (!IsStdArray<U>::value)
inline constexpr auto MakeMD(const std::array<U, N>& array) {
  return MDInitList<T, U, U, N>(array);
}

template <typename T, typename U, size_t N>
  requires IsStdArray<std::remove_cvref_t<U>>::value
inline constexpr auto MakeMD(const std::array<U, N>& array) {
  return typename MDInitListHelper<T, U, U, N>::mdlist_type(array);
}

template <size_t... UInts>
inline constexpr size_t Max() {
  size_t max_val = 0;  // returing 0 when sizeof...(UInts) == 0
  ((max_val = max_val < UInts ? UInts : max_val), ...);
  return max_val;
}

}  // namespace pp1

#endif