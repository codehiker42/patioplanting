#ifndef _PP1_INDEX_H_
#define _PP1_INDEX_H_

#include <cassert>
#include <cmath>
#include <optional>
#include <utility>

namespace pp1 {

template <size_t... Indexes>
struct Indices {
  constexpr Indices() noexcept;

  constexpr static size_t Size() noexcept;
};

template <size_t... Indexes>
constexpr Indices<Indexes...>::Indices() noexcept = default;

template <size_t... Indexes>
constexpr size_t Indices<Indexes...>::Size() noexcept {
  return sizeof...(Indexes);
}

template <int... SigIndexes>
struct SigIndices {
  constexpr SigIndices() noexcept;
  constexpr static size_t Size() noexcept;

  template <size_t Dim>
  constexpr static Indices<static_cast<size_t>(SigIndexes < 0 ? Dim + SigIndexes
                                                              : SigIndexes)...>
  ToIndices() noexcept;
};

template <int... SigIndexes>
constexpr SigIndices<SigIndexes...>::SigIndices() noexcept = default;

template <int... SigIndexes>
constexpr size_t SigIndices<SigIndexes...>::Size() noexcept {
  return sizeof...(SigIndexes);
}

template <int... SigIndexes>
template <size_t Dim>
constexpr Indices<static_cast<size_t>(SigIndexes < 0 ? Dim + SigIndexes
                                                     : SigIndexes)...>
SigIndices<SigIndexes...>::ToIndices() noexcept {
  static_assert(
      (((SigIndexes >= 0 && static_cast<size_t>(SigIndexes) < Dim) ||
        (SigIndexes < 0 && static_cast<size_t>(-(SigIndexes + 1)) <= Dim)) &&
       ...),
      "Out of bound");
  return {};
}

template <std::size_t N>
struct SliceNotation {
  constexpr SliceNotation(const char (&notation)[N]) noexcept;

  constexpr std::optional<int> ToNumber(const char* begin,
                                        const char* end) noexcept;

  bool has_from_{};
  int from_{};
  bool has_until_{};
  int until_{};
  int by_{1};
};

template <std::size_t N>
constexpr SliceNotation<N>::SliceNotation(const char (&notation)[N]) noexcept {
  int colon_index[2] = {-1, -1};
  for (int i = 0, colon_idx = 0; i < N; ++i) {
    char ch = notation[i];
    if (ch == ':') {
      assert(colon_idx <= 1 && "Invalid syntax, colon usage");
      colon_index[colon_idx++] = i;
    } else if (ch != ' ' && (ch < '0' && ch > '9') && ch != '-' && ch != '+') {
      assert("Invalid syntax, unexpected char");
    }
  }

  const size_t f_end = colon_index[0] == -1 ? N : colon_index[0];
  const auto from_op = ToNumber(notation, notation + f_end);
  if (from_op.has_value()) {
    has_from_ = true;
    from_ = *from_op;
  }
  if (colon_index[0] != -1) {
    const size_t u_end = colon_index[1] == -1 ? N : colon_index[1];
    const auto until_op =
        ToNumber(notation + colon_index[0] + 1, notation + u_end);
    if (until_op.has_value()) {
      has_until_ = true;
      until_ = *until_op;
    }
  }
  if (colon_index[1] != -1) {
    const auto by_op = ToNumber(notation + colon_index[1] + 1, notation + N);
    if (by_op.has_value()) {
      by_ = *by_op;
    }
  }
}

template <std::size_t N>
constexpr std::optional<int> SliceNotation<N>::ToNumber(
    const char* begin, const char* end) noexcept {
  while (begin != end && *begin == ' ') {
    ++begin;
  }
  bool digit = false;
  int sign = 1, num = 0;
  while (begin != end && *begin != ' ') {
    const char ch = *begin++;
    if (digit && (ch == '-' || ch == '+')) {
      assert("Invalid syntax, a sign operator");
    } else if (ch == '-') {
      sign *= -1;
    } else if (ch >= '0' && ch <= '9') {
      digit = true;
      num = num * 10 + (ch - '0');
    }
  }
  while (begin != end && digit) {
    const char ch = *begin++;
    if (ch != ' ') {
      assert("Invalid syntax, a space between numeric expression");
    }
  }
  if (digit) {
    return {num * sign};
  } else {
    return {};
  }
}

template <typename T>
struct IndexPack;

template <typename T, size_t... Seq>
struct IndexPack<std::integer_sequence<T, Seq...>> {
  using type = Indices<Seq...>;
};

template <size_t From, int By, typename>
struct IndexSequence;

template <size_t From, int By, size_t... Seqs>
struct IndexSequence<From, By, std::integer_sequence<size_t, Seqs...>> {
  using type = std::integer_sequence<size_t, (From + Seqs * By)...>;
};

template <size_t From, size_t N, int By>
struct SliceUnPack {
  using type = IndexPack<typename IndexSequence<
      From, By, std::make_integer_sequence<size_t, N>>::type>::type;
};

template <size_t Dim, bool HasFrom, int From, int By>
inline constexpr size_t NormFrom() {
  static_assert(!HasFrom || ((From >= 0 && From < Dim) ||
                             (From < 0 && (Dim + From) >= 0)),
                "Out of range: From");
  return HasFrom ? (From >= 0 ? From : Dim + From) : (By > 0 ? 0U : Dim - 1);
}

template <size_t Dim, bool HasFrom, int From, bool HasUntil, int Until, int By>
inline constexpr size_t SeqSizeN() {
  static_assert(!HasUntil || ((Until >= 0 && Until <= Dim) ||
                              (Until < 0 && (Dim + Until) >= 0)),
                "Out of range: Until");
  constexpr size_t u_from = NormFrom<Dim, HasFrom, From, By>();
  constexpr size_t u_inc_to =
      HasUntil ? ((Until >= 0 ? Until : Dim + Until) + (By > 0 ? -1 : +1))
               : (By > 0 ? Dim - 1 : 0U);

  if ((By > 0 && u_from > u_inc_to) || (By < 0 && u_from < u_inc_to)) {
    return 0UL;
  } else {
    constexpr size_t sentienl_n =
        std::max(u_from, u_inc_to) - std::min(u_from, u_inc_to) + 1;
    return (sentienl_n + std::abs(By) - 1) / std::abs(By);
  }
}

template <SliceNotation Notation, bool HasFrom = Notation.has_from_,
          int From = Notation.from_, bool HasUntil = Notation.has_until_,
          int Until = Notation.until_, int By = Notation.by_>
struct Slice {
  template <size_t Dim>
  using SliceIndexReturnType =
      typename SliceUnPack<NormFrom<Dim, HasFrom, From, By>(),
                           SeqSizeN<Dim, HasFrom, From, HasUntil, Until, By>(),
                           By>::type;

  template <size_t Dim>
  constexpr static SliceIndexReturnType<Dim> ToIndices() noexcept;
};

template <SliceNotation Notation, bool HasFrom, int From, bool HasUntil,
          int Until, int By>
template <size_t Dim>
constexpr Slice<Notation, HasFrom, From, HasUntil, Until,
                By>::SliceIndexReturnType<Dim>
Slice<Notation, HasFrom, From, HasUntil, Until, By>::ToIndices() noexcept {
  return {};
}

namespace Dim {
inline static constexpr Slice All = Slice<"::">();
inline static constexpr Slice Reverse = Slice<"::-1">();
}  // namespace Dim

}  // namespace pp1
#endif
