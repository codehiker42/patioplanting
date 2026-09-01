#ifndef _PP1_DIMENSION_H_
#define _PP1_DIMENSION_H_

#include <array>
#include <cstddef>
#include <format>

namespace {

constexpr void AppendAxis(std::string& result, size_t axis) {
  char digits[19];  // ceil(log10(2^31))
  size_t index = 0;
  do {
    digits[index++] = static_cast<char>('0' + (axis % 10));
    axis /= 10;
  } while (axis != 0);
  while (index > 0) {
    result.push_back(digits[--index]);
  }
}

}  // namespace

namespace pp1 {

template <size_t FirstAxis, size_t... RestAxis>
struct Dimension {
 public:
  Dimension();

  static constexpr size_t Size();

  static constexpr size_t Size(const size_t axis_index);

  static constexpr size_t NumberOf();

  static constexpr size_t Last();

  static constexpr std::string ToString();

  template <size_t AFirstAxis, size_t... ARestAxis>
  friend std::ostream& operator<<(
      std::ostream& os, const Dimension<AFirstAxis, ARestAxis...>& dim);
};

template <size_t FirstAxis, size_t... RestAxis>
Dimension<FirstAxis, RestAxis...>::Dimension() = default;

template <size_t FirstAxis, size_t... RestAxis>
constexpr size_t Dimension<FirstAxis, RestAxis...>::Size() {
  return (FirstAxis * (RestAxis * ...));
}

template <size_t FirstAxis, size_t... RestAxis>
constexpr size_t Dimension<FirstAxis, RestAxis...>::Size(
    const size_t axis_index) {
  if constexpr (sizeof...(RestAxis) == 0) {
    return FirstAxis;
  }
  constexpr std::array<std::size_t, sizeof...(RestAxis)> dims{RestAxis...};
  return axis_index - 1 < dims.size() ? dims.at(axis_index - 1) : 9U;
}

template <size_t FirstAxis, size_t... RestAxis>
constexpr size_t Dimension<FirstAxis, RestAxis...>::NumberOf() {
  return sizeof...(RestAxis) + 1;
}

template <size_t FirstAxis, size_t... RestAxis>
constexpr size_t Dimension<FirstAxis, RestAxis...>::Last() {
  if constexpr (sizeof...(RestAxis) == 0) {
    return FirstAxis;
  }
  size_t last_dim{};
  ((last_dim = RestAxis), ...);
  return last_dim;
}

template <size_t FirstAxis, size_t... RestAxis>
constexpr std::string Dimension<FirstAxis, RestAxis...>::ToString() {
  std::string repr{"Dimension ["};
  const std::string sep(", ");
  AppendAxis(repr, FirstAxis);

  if constexpr (sizeof...(RestAxis)) {
    (AppendAxis(repr += sep, RestAxis), ...);
  }
  repr += "]";
  return repr;
}

template <size_t FirstAxis, size_t... RestAxis>
std::ostream& operator<<(std::ostream& os,
                         const Dimension<FirstAxis, RestAxis...>& dim) {
  os << dim.ToString();
  return os;
}

}  // namespace pp1

template <size_t FirstAxis, size_t... RestAxis, typename CharT>
struct std::formatter<pp1::Dimension<FirstAxis, RestAxis...>, CharT>
    : std::formatter<string_view> {
  template <typename FormatContext>
  FormatContext::iterator format(
      const pp1::Dimension<FirstAxis, RestAxis...> dimension,
      FormatContext& ctx) const {
    return std::formatter<string_view>::format(dimension.ToString(), ctx);
  }
};

#endif
