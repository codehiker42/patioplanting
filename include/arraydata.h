#ifndef _PP1_ARRAYDATA_H_
#define _PP1_ARRAYDATA_H_

#include <cstdlib>

#include "dimension.h"
#include "global_constants.h"
#include "pp1_type.h"

namespace {

inline size_t CalcPadding(size_t size) {
  return (size + kCacheLineSize - 1) / kCacheLineSize * kCacheLineSize;
}

}  // namespace

namespace pp1 {

enum class AllocationType { MainMemoryPacked, MainMemoryAligned };

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

  ArrayData();
  template <size_t FirstAxis, size_t... RestAxis>
  ArrayData(const Dimension<FirstAxis, RestAxis...>& dim,
            const AllocationType alloc_type = AllocationType::MainMemoryPacked);
  ArrayData(const ArrayData&) = delete;
  ArrayData& operator=(const ArrayData&) = delete;
  ArrayData(ArrayData&& rhs);
  ~ArrayData();

  template <typename InputIt>
  void FillIn(InputIt begin, InputIt end);

 private:
  AllocationType alloc_type_{AllocationType::MainMemoryPacked};
  size_t last_dim_{0};
  size_t n_elems_{0};
  size_t stride_size_{0};
  T* buffer_{nullptr};
};

template <typename T>
ArrayData<T>::ArrayData() = default;

template <typename T>
template <size_t FirstAxis, size_t... RestAxis>
ArrayData<T>::ArrayData(const Dimension<FirstAxis, RestAxis...>& dim,
                        const AllocationType alloc_type)
    : alloc_type_(alloc_type), last_dim_(dim.Last()), n_elems_(dim.Size()) {
  switch (alloc_type) {
    case AllocationType::MainMemoryPacked:
      buffer_ = std::malloc(n_elems_ * sizeof(T));
      stride_size_ = last_dim_;
      break;
    case AllocationType::MainMemoryAligned:
      stride_size_ = CalcPadding(last_dim_ * sizeof(T)) / sizeof(T);
      const size_t buf_size = n_elems_ / last_dim_ * stride_size_ * sizeof(T);
      buffer_ = std::aligned_alloc(kCacheLineSize, buf_size);
      break;
    default:
      throw std::logic_error();
  }

  if (!buffer_) {
    throw std::bad_alloc();
  }
}

template <typename T>
ArrayData<T>::~ArrayData() {
  if (buffer_) {
    std::free(buffer_);
  }
}

template <typename T>
template <typename InputIt>
void ArrayData<T>::FillIn(InputIt begin, InputIt end) {
  static_assert(stride_size_ >= last_dim_, "Illegal state");

  const size_t offset = stride_size_ - last_dim_;
  InputIt iter = begin;
  for (size_t i = 0, buf_index = 0; i < n_elems_ && iter != end;
       ++i, ++buf_index, ++iter) {
    if (buf_index != 0 && (buf_index % last_dim_) == 0) {
      buf_index += offset;
    }
    *(buffer_ + buf_index) = *iter;
  }
}

}  // namespace pp1
#endif