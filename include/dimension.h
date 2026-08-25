#ifndef _PP1_DIMENSION_H_
#define _PP1_DIMENSION_H_

#include <cstddef>
#include <initializer_list>
#include <vector>

namespace pp1 {

class Dimension {
 public:
  Dimension() = default;
  Dimension(std::initializer_list<size_t> elems);

  size_t Size() const;

  size_t Size(const size_t axis_index) const;

  size_t NumberOf() const;

  bool operator==(const Dimension& another) const;
  bool operator!=(const Dimension& another) const;

 private:
  std::vector<size_t> dim_elems_;
};

}  // namespace pp1
#endif
