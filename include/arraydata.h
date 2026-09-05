#ifndef _PP1_ARRAYDATA_H_
#define _PP1_ARRAYDATA_H_

#include <cassert>
#include <cstdlib>
#include <stdexcept>
#include <tuple>
#include <type_traits>

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
  ArrayData(const AllocationType alloc_type,
            const Dimension<FirstAxis, RestAxis...>& dim);

  template <size_t FirstAxis, size_t... RestAxis>
  ArrayData(const AllocationType alloc_type,
            const Dimension<FirstAxis, RestAxis...>& dim, T&& init)
    requires std::copyable<T> && std::is_destructible_v<T>;

  template <size_t FirstAxis, size_t... RestAxis, std::input_iterator IT>
  ArrayData(const AllocationType alloc_type,
            const Dimension<FirstAxis, RestAxis...>& dim, IT begin, IT end)
    requires std::copyable<T> && std::is_destructible_v<T>;

  template <size_t FirstAxis, size_t... RestAxis, typename... Args,
            typename FirstTuple, typename... RestTuples>
  ArrayData(const AllocationType alloc_type,
            const Dimension<FirstAxis, RestAxis...>& dim,
            FirstTuple&& first_elem, RestTuples&&... rest_elems)
    requires TupleForT<T, FirstTuple> && (TupleForT<T, RestTuples> && ...);

  ArrayData(const ArrayData&) = delete;
  ArrayData& operator=(const ArrayData&) = delete;
  ArrayData(ArrayData&& rhs);
  ~ArrayData();

  template <std::input_iterator InputIt>
  void FillIn(InputIt begin, InputIt end)
    requires std::copyable<T> && std::is_destructible_v<T>;

  template <typename Self>
  decltype(auto) NthElement(this Self&&, size_t n_th);

 private:
  template <std::invocable<pointer> AcEach, std::invocable Senti,
            std::invocable<pointer> AcUndo>
  void ForEach(AcEach&& each_ac, Senti&& senti, AcUndo&& undo_ac);

  constexpr pointer AddrOf(size_t n_th) const noexcept;

  bool t_constructed_{false};
  AllocationType alloc_type_{AllocationType::MainMemoryPacked};
  size_t last_dim_{0};
  size_t n_elems_{0};
  size_t stride_size_{0};
  size_t sentinel_{0};
  T* buffer_{nullptr};
};

template <typename T>
ArrayData<T>::ArrayData() = default;

template <typename T>
template <size_t FirstAxis, size_t... RestAxis>
ArrayData<T>::ArrayData(const AllocationType alloc_type,
                        const Dimension<FirstAxis, RestAxis...>& dim)
    : alloc_type_(alloc_type), last_dim_(dim.Last()), n_elems_(dim.Size()) {
  switch (alloc_type) {
    case AllocationType::MainMemoryPacked: {
      stride_size_ = last_dim_;
      sentinel_ = n_elems_;
      buffer_ = static_cast<pointer>(
          ::operator new(n_elems_ * sizeof(T), std::align_val_t{alignof(T)}));

      break;
    }
    case AllocationType::MainMemoryAligned: {
      stride_size_ = CalcPadding(last_dim_ * sizeof(T)) / sizeof(T);
      sentinel_ = n_elems_ / last_dim_ * stride_size_;
      buffer_ = static_cast<pointer>(
          std::aligned_alloc(kCacheLineSize, sentinel_ * sizeof(T)));
      break;
    }
    default:
      throw std::logic_error("Illegal state");
  }

  assert(stride_size_ >= last_dim_);
  if (!buffer_) {
    throw std::bad_alloc();
  }
}

template <typename T>
template <size_t FirstAxis, size_t... RestAxis>
ArrayData<T>::ArrayData(const AllocationType alloc_type,
                        const Dimension<FirstAxis, RestAxis...>& dim, T&& init)
  requires std::copyable<T> && std::is_destructible_v<T>
    : ArrayData(alloc_type, dim) {
  ForEach(
      [&](pointer p_t) {
        if constexpr (std::is_trivially_copyable_v<T>) {
          *p_t = std::forward<T>(init);
        } else {
          std::construct_at(p_t, std::forward<T>(init));
        }
      },
      []() { return true; },
      [](pointer p_t) {
        if constexpr (!std::is_trivially_copyable_v<T>) {
          std::destroy_at(p_t);
        }
      });
  t_constructed_ = true;
}

template <typename T>
template <size_t FirstAxis, size_t... RestAxis, std::input_iterator IT>
ArrayData<T>::ArrayData(const AllocationType alloc_type,
                        const Dimension<FirstAxis, RestAxis...>& dim, IT begin,
                        IT end)
  requires std::copyable<T> && std::is_destructible_v<T>
    : ArrayData(alloc_type, dim) {
  IT iter = begin;
  bool stop_copying = false;
  ForEach(
      [&](pointer p_t) {
        if (iter != end) {
          *p_t = *iter++;
        } else if constexpr (std::is_default_constructible_v<T>) {
          std::construct_at<T>(p_t);
        } else {
          stop_copying = true;
        }
      },
      [&]() { return !stop_copying; },
      [](pointer p_t) { std::destroy_at(p_t); });
  t_constructed_ = true;
}

template <typename T>
template <size_t FirstAxis, size_t... RestAxis, typename... Args,
          typename FirstTuple, typename... RestTuples>
ArrayData<T>::ArrayData(const AllocationType alloc_type,
                        const Dimension<FirstAxis, RestAxis...>& dim,
                        FirstTuple&& first_elem, RestTuples&&... rest_elems)
  requires TupleForT<T, FirstTuple> && (TupleForT<T, RestTuples> && ...)
    : ArrayData(alloc_type, dim) {
  auto FromTuple = [&](size_t n_th, auto&& tuple) {
    std::apply(
        [&](auto&&... args) { std::construct_at<T>(AddrOf(n_th), args...); },
        tuple);
  };

  size_t n_creation = 0;
  try {
    FromTuple(n_creation++, std::forward<FirstTuple>(first_elem));
    if constexpr (sizeof...(RestTuples) > 0) {
      ((FromTuple(n_creation++, std::forward<RestTuples>(rest_elems))), ...);
    }
    if constexpr (std::is_default_constructible_v<T>) {
      for (size_t i = n_creation; i < n_elems_; ++i) {
        std::construct_at<T>(AddrOf(i));
      }
    }
  } catch (...) {
    for (size_t i = 0; i < n_creation; ++i) {
      std::destroy_at<T>(AddrOf(i));
    }
    throw;
  }
  t_constructed_ = true;
}

template <typename T>
ArrayData<T>::~ArrayData() {
  if (buffer_ == 0) {
    return;
  }

  const size_t offset = stride_size_ - last_dim_;
  for (size_t i = 0, buf_index = 0; t_constructed_ && i < n_elems_;
       ++i, ++buf_index) {
    if (buf_index != 0 && (i % last_dim_) == 0) {
      buf_index += offset;
    }
    std::destroy_at(buffer_ + buf_index);
  }

  switch (alloc_type_) {
    case AllocationType::MainMemoryPacked:
      ::operator delete(buffer_, sentinel_ * sizeof(T),
                        std::align_val_t{alignof(T)});
      break;
    case AllocationType::MainMemoryAligned:
      std::free(buffer_);
      break;
  }
}

template <typename T>
template <std::input_iterator IT>
void ArrayData<T>::FillIn(IT begin, IT end)
  requires std::copyable<T> && std::is_destructible_v<T>
{
  IT iter = begin;
  ForEach([&](pointer p_t) { *p_t = *iter++; }, [&]() { return iter != end; },
          [](pointer p_t) { std::destroy_at(p_t); });
}

template <typename T>
template <typename Self>
decltype(auto) ArrayData<T>::NthElement(this Self&& self, size_t n_th) {
  const size_t index =
      (n_th / self.last_dim_) * self.stride_size_ + (n_th % self.last_dim_);
  if (index >= self.sentinel_) {
    throw std::out_of_range("Out of range");
  }
  return *(std::forward<Self>(self).buffer_ + index);
}

template <typename T>
template <std::invocable<typename ArrayData<T>::pointer> AcEach,
          std::invocable Senti,
          std::invocable<typename ArrayData<T>::pointer> AcUndo>
void ArrayData<T>::ForEach(AcEach&& each_ac, Senti&& senti, AcUndo&& undo_ac) {
  const size_t offset = stride_size_ - last_dim_;
  size_t n_succ = 0;
  try {
    for (size_t i = 0, buf_index = 0; senti() && i < n_elems_;
         ++i, ++buf_index) {
      if (buf_index != 0 && (i % last_dim_) == 0) {
        buf_index += offset;
      }
      each_ac(buffer_ + buf_index);
      ++n_succ;
    }
  } catch (...) {
    for (size_t i = 0, buf_index = 0; i < n_succ; ++i, ++buf_index) {
      if (buf_index != 0 && (i % last_dim_) == 0) {
        buf_index += offset;
      }
      undo_ac(buffer_ + buf_index);
    }
    throw;
  }
}

template <typename T>
constexpr ArrayData<T>::pointer ArrayData<T>::AddrOf(
    size_t n_th) const noexcept {
  const size_t index = (n_th / last_dim_) * stride_size_ + (n_th % last_dim_);
  return buffer_ + index;
}

}  // namespace pp1
#endif