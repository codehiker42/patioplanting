#ifndef _PP1_INDEX_H_
#define _PP1_INDEX_H_

#include <optional>

namespace pp1 {

class Index {
 public:
  constexpr Index(std::optional<signed> from = std::nullopt,
                  std::optional<signed> to = std::nullopt,
                  std::optional<signed> by = std::nullopt);

  Index& To(signed to);
  Index& By(signed by);

 private:
  std::optional<signed> from_;
  std::optional<signed> to_;
  std::optional<signed> by_;
};

constexpr Index::Index(std::optional<signed> from, std::optional<signed> to,
             std::optional<signed> by)
    : from_(from), to_(to), by_(by) {}

Index& Index::To(signed to) {
  to_ = to;
  return *this;
}

Index& Index::By(signed by) {
  by_ = by;
  return *this;
}

namespace Dim {
  inline static constexpr Index All = Index();
  inline static constexpr Index Reverse = Index(std::nullopt, std::nullopt, -1);
} 

}  // namespace pp1
#endif
