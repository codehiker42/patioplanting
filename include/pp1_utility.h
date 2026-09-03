#ifndef _PP1_UTILITY_H_
#define _PP1_UTILITY_H_

#include <tuple>

namespace pp1 {

template <typename... Args>
inline constexpr auto arguments(Args&&... args) {
  return std::forward_as_tuple(args...);
}

}  // namespace pp1

#endif