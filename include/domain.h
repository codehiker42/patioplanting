#ifndef _PP1_DOMAIN_H_
#define _PP1_DOMAIN_H_

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

template <size_t FirstDimension, size_t... RestDimensions>
struct Domain {
 public:
  Domain();

  static constexpr size_t Size();

  static constexpr size_t Size(const size_t axis_index);

  static constexpr size_t Rank();

  static constexpr size_t Last();

  static constexpr std::string ToString();

  template <size_t AFirstDimension, size_t... ARestDimensions>
  friend std::ostream& operator<<(
      std::ostream& os, const Domain<AFirstDimension, ARestDimensions...>& dim);
};

template <size_t FirstDimension, size_t... RestDimensions>
Domain<FirstDimension, RestDimensions...>::Domain() = default;

template <size_t FirstDimension, size_t... RestDimensions>
constexpr size_t Domain<FirstDimension, RestDimensions...>::Size() {
  if constexpr (sizeof...(RestDimensions)) {
    return (FirstDimension * (RestDimensions * ...));
  } else {
    return FirstDimension;
  }
}

template <size_t FirstDimension, size_t... RestDimensions>
constexpr size_t Domain<FirstDimension, RestDimensions...>::Size(
    const size_t axis_index) {
  if constexpr (sizeof...(RestDimensions) == 0) {
    return FirstDimension;
  }
  constexpr std::array<std::size_t, sizeof...(RestDimensions)> dims{
      RestDimensions...};
  return axis_index - 1 < dims.size() ? dims.at(axis_index - 1) : 9U;
}

template <size_t FirstDimension, size_t... RestDimensions>
constexpr size_t Domain<FirstDimension, RestDimensions...>::Rank() {
  return sizeof...(RestDimensions) + 1;
}

template <size_t FirstDimension, size_t... RestDimensions>
constexpr size_t Domain<FirstDimension, RestDimensions...>::Last() {
  if constexpr (sizeof...(RestDimensions) == 0) {
    return FirstDimension;
  }
  size_t last_dim{};
  ((last_dim = RestDimensions), ...);
  return last_dim;
}

template <size_t FirstDimension, size_t... RestDimensions>
constexpr std::string Domain<FirstDimension, RestDimensions...>::ToString() {
  std::string repr{"Domain ["};
  const std::string sep(", ");
  AppendAxis(repr, FirstDimension);

  if constexpr (sizeof...(RestDimensions)) {
    (AppendAxis(repr += sep, RestDimensions), ...);
  }
  repr += "]";
  return repr;
}

template <size_t FirstDimension, size_t... RestDimensions>
std::ostream& operator<<(std::ostream& os,
                         const Domain<FirstDimension, RestDimensions...>& dim) {
  os << dim.ToString();
  return os;
}

}  // namespace pp1

template <size_t FirstDimension, size_t... RestDimensions, typename CharT>
struct std::formatter<pp1::Domain<FirstDimension, RestDimensions...>, CharT>
    : std::formatter<string_view> {
  template <typename FormatContext>
  FormatContext::iterator format(
      const pp1::Domain<FirstDimension, RestDimensions...> domain,
      FormatContext& ctx) const {
    return std::formatter<string_view>::format(domain.ToString(), ctx);
  }
};

#endif
