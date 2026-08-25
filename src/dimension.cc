#include "dimension.h"

#include <numeric>

namespace pp1 {

Dimension::Dimension(std::initializer_list<size_t> elems) : dim_elems_(elems) {}

size_t Dimension::Size() const {
  return std::accumulate(dim_elems_.begin(), dim_elems_.end(), 1ULL,
                         std::multiplies<>());
}

size_t Dimension::Size(const size_t axis_index) const {
  return axis_index >= dim_elems_.size() ? 0 : dim_elems_.at(axis_index);
}

size_t Dimension::NumberOf() const { return dim_elems_.size(); }

bool Dimension::operator==(const Dimension& another) const {
  return this->dim_elems_ == another.dim_elems_;
}

bool Dimension::operator!=(const Dimension& another) const {
  return this->dim_elems_ != another.dim_elems_;
}

}  // namespace pp1
