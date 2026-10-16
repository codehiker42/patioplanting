#ifndef _PP1_INDEX_H_
#define _PP1_INDEX_H_

#include <cassert>
#include <initializer_list>
#include <optional>
#include <string>
#include <vector>

namespace pp1 {

class Index {
 public:
  enum class Type { Sequence, RandomAccessing };
  constexpr Index(std::optional<signed> from = std::nullopt,
                  std::optional<signed> until = std::nullopt,
                  std::optional<signed> by = std::nullopt);

  constexpr Index(std::initializer_list<signed> list);

  constexpr Index& Until(const signed until);

  constexpr Index& By(const signed by);

  constexpr static size_t Norm(const signed signed_index, const size_t n_dim);

  constexpr std::vector<size_t> NormIndices(const size_t n_dim) const;

 private:
  const Type type_;
  std::optional<signed> from_;
  std::optional<signed> until_;
  std::optional<signed> by_;
  const std::vector<signed> indices_;
};

constexpr Index::Index(std::optional<signed> from, std::optional<signed> until,
                       std::optional<signed> by)
    : type_(Type::Sequence), from_(from), until_(until), by_(by) {}

constexpr Index::Index(std::initializer_list<signed> list)
    : type_(Type::RandomAccessing), indices_(list) {}

constexpr Index& Index::Until(const signed until) {
  assert(this->type_ == Type::Sequence);
  until_ = until;
  return *this;
}

constexpr Index& Index::By(const signed by) {
  assert(this->type_ == Type::Sequence);
  assert(by != 0);
  by_ = by;
  return *this;
}

constexpr size_t Index::Norm(const signed signed_index, const size_t n_dim) {
  const size_t signed_dim = static_cast<signed>(n_dim);
  const signed normalised =
      signed_index >= 0 ? signed_index : signed_dim + signed_index;

  assert((normalised >= 0 && normalised < signed_dim) &&
         (std::string("Index ") + std::to_string(signed_index) +
          " is out of range")
             .c_str());

  return static_cast<size_t>(normalised);
}

constexpr std::vector<size_t> Index::NormIndices(const size_t n_dim) const {
  std::vector<size_t> indices;
  if (type_ == Type::RandomAccessing) {
    for (signed signed_index : indices_) {
      indices.emplace_back(Norm(signed_index, n_dim));
    }
    return indices;
  }
  const size_t signed_dim = static_cast<signed>(n_dim);
  assert((!until_.has_value() || (*until_ >= 0 && *until_ < signed_dim) ||
          (*until_ < 0 && *until_ + signed_dim >= 0)) &&
         "Invalid until_ for a given n_dim");
  const signed by = by_.value_or(1);
  for (signed i = from_.value_or(by > 0 ? 0 : signed_dim - 1),
              until = until_.value_or(by > 0 ? signed_dim : -1);
       (by > 0 && i < until) || (by < 0 && i > until); i += by) {
    indices.emplace_back(Norm(i, n_dim));
  }
  return indices;
}

namespace Dim {
inline static constexpr Index All = Index();
inline static constexpr Index Reverse = Index(std::nullopt, std::nullopt, -1);
}  // namespace Dim

}  // namespace pp1
#endif
