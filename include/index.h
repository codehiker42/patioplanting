#ifndef _PP1_INDEX_H_
#define _PP1_INDEX_H_

#include <cassert>
#include <initializer_list>
#include <optional>
#include <vector>

namespace pp1 {

class Index {
 public:
  enum class Type { Sequence, RandomAccessing };
  constexpr Index(std::optional<signed> from = std::nullopt,
                  std::optional<signed> to = std::nullopt,
                  std::optional<signed> by = std::nullopt);

  constexpr Index(std::initializer_list<signed> list);

  constexpr Index& To(const signed to);
  constexpr Index& By(const signed by);

  constexpr Type IndexingType() const;

  constexpr std::optional<signed> From() const;
  constexpr std::optional<signed> To() const;
  constexpr std::optional<signed> By() const;

  constexpr const std::vector<signed>& Indices() const;

 private:
  const Type type_;
  std::optional<signed> from_;
  std::optional<signed> to_;
  std::optional<signed> by_;
  const std::vector<signed> indices_;
};

constexpr Index::Index(std::optional<signed> from, std::optional<signed> to,
                       std::optional<signed> by)
    : type_(Type::Sequence), from_(from), to_(to), by_(by) {}

constexpr Index::Index(std::initializer_list<signed> list)
    : type_(Type::RandomAccessing), indices_(list) {}

constexpr Index& Index::To(const signed to) {
  assert(this->type_ == Type::Sequence);
  to_ = to;
  return *this;
}

constexpr Index& Index::By(const signed by) {
  assert(this->type_ == Type::Sequence);
  by_ = by;
  return *this;
}

constexpr Index::Type Index::IndexingType() const { return type_; }

constexpr std::optional<signed> Index::From() const {
  assert(this->type_ == Type::Sequence);
  return from_;
}

constexpr std::optional<signed> Index::To() const {
  assert(this->type_ == Type::Sequence);
  return to_;
}

constexpr std::optional<signed> Index::By() const {
  assert(this->type_ == Type::Sequence);
  return by_;
}

constexpr const std::vector<signed>& Index::Indices() const {
  assert(this->type_ == Type::RandomAccessing);
  return indices_;
}

namespace Dim {
inline static constexpr Index All = Index();
inline static constexpr Index Reverse = Index(std::nullopt, std::nullopt, -1);
}  // namespace Dim

}  // namespace pp1
#endif
