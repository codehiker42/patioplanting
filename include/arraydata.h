#ifndef _PP1_ARRAYDATA_H_
#define _PP1_ARRAYDATA_H_

#include <cstdlib>
#include <stdexcept>

#include "dimension.h"
#include "global_constants.h"
#include "pp1_type.h"

namespace {

inline size_t CalcPadding(size_t size) {
  return (size + kCacheLineSize - 1) / kCacheLineSize * kCacheLineSize;
}

}  // namespace

namespace pp1 {

template <typename T>
class ArrayData {
 public:
  using reference = T&;
  using const_reference = std::add_const_t<T>&;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using const_pointer = std::add_const_t<T>*;

  enum class AllocationType { MainMemoryPacked, MainMemoryAligned };

  ArrayData();
  template <size_t FirstAxis, size_t... RestAxis>
  ArrayData(const Dimension<FirstAxis, RestAxis...>& dim,
            const AllocationType alloc_type = AllocationType::MainMemoryPacked);
  ArrayData(const ArrayData&) = delete;
  ArrayData& operator=(const ArrayData&) = delete;
  ArrayData(ArrayData&& rhs);
  ~ArrayData();

 private:
  T* buffer_{nullptr};
  size_t stride_size_{0};
  AllocationType alloc_type_{AllocationType::MainMemoryPacked};
};

template <typename T>
ArrayData<T>::ArrayData() = default;

template <typename T>
template <size_t FirstAxis, size_t... RestAxis>
ArrayData<T>::ArrayData(const Dimension<FirstAxis, RestAxis...>& dim,
                        const AllocationType alloc_type)
    : alloc_type_(alloc_type) {
  constexpr size_t n_elems = dim.Size();
  constexpr size_t last_dim = dim.Last();

  switch (alloc_type) {
    case AllocationType::MainMemoryPacked:
      buffer_ = std::malloc(n_elems * sizeof(T));
      stride_size_ = last_dim;
      break;
    case AllocationType::MainMemoryAligned:
      stride_size_ = CalcPadding(last_dim * sizeof(T)) / sizeof(T);
      const size_t buf_size = dim.Size() / last_dim * stride_size_ * sizeof(T);
      buffer_ = std::aligned_alloc(kCacheLineSize, buf_size);
      break;
    default:
      throw std::logic_error();
  }
}

template <typename T>
ArrayData<T>::~ArrayData() {
  if (buffer_) {
    std::free(buffer_);
  }
}

}  // namespace pp1
#endif