#ifndef _PP1_ARRAY_H_
#define _PP1_ARRAY_H_

#include <algorithm>
#include <array>
#include <initializer_list>
#include <memory>
#include <vector>

#include "arraydata.h"
#include "domain.h"
#include "index.h"
#include "pp1_type.h"
#include "pp1_utility.h"

namespace pp1 {

template <typename T = Types::DefaultRealT, size_t FirstDimension = 1,
          size_t... RestDimensions>
class Array {
 public:
  using reference = T&;
  using const_reference = std::add_const_t<T>&;
  using size_type = size_t;
  using difference_type = ptrdiff_t;
  using value_type = T;
  using pointer = T*;
  using const_pointer = std::add_const_t<T>*;
  using domain = Domain<FirstDimension, RestDimensions...>;

  constexpr Array(const Array& other) noexcept;
  constexpr Array(const Array&& other) noexcept;

  constexpr Array& operator=(const Array& other) noexcept;
  constexpr Array& operator=(Array&& other) noexcept;

  constexpr Array(
      const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::default_initializable<T>;

  template <std::input_iterator IT>
  constexpr Array(
      IT begin, IT end,
      const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::copyable<T>;

  constexpr Array(
      std::initializer_list<T> init_list,
      const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::copyable<T>;

  constexpr Array(
      const std::vector<T>& from_vec,
      const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::copyable<T>;

  constexpr Array(
      T&& t, const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::copyable<T>;

  template <typename FirstTuple, typename... RestTuples>
  constexpr Array(const AllocationType alloc_type, FirstTuple&& first_elem,
                  RestTuples&&... rest_elems)
    requires TupleForT<T, FirstTuple> && (TupleForT<T, RestTuples> && ...);

  template <typename U, typename V>
  constexpr Array(
      const MDInitList<T, U, V, FirstDimension, RestDimensions...>&
          md_init_list,
      const AllocationType alloc_type = AllocationType::MainMemoryPacked);

  constexpr Array(
      const Seq<T, FirstDimension, RestDimensions...>& seq,
      const AllocationType alloc_type = AllocationType::MainMemoryPacked)
    requires std::semiregular<T> && std::equality_comparable<T>;

  constexpr Array(std::shared_ptr<ArrayData<T>> data,
                  const std::vector<size_t> strides,
                  const std::vector<std::vector<size_t>> offsets) noexcept;

  template <size_t RhsFirstDimension, size_t... RhsRestDimensions>
  constexpr Array<T, RhsFirstDimension, RhsRestDimensions...> Reshape(
      const Domain<RhsFirstDimension, RhsRestDimensions...>& dim_to_shape);

  template <typename Self>
  constexpr T& operator[](this Self&&, const size_t index)
    requires(sizeof...(RestDimensions) == 0);

  constexpr Array<T, RestDimensions...> operator[](const size_t index)
    requires(sizeof...(RestDimensions) != 0);

  template <size_t... Indexes>
  constexpr Array<T, (sizeof...(Indexes)), RestDimensions...> operator[](
      const Indices<Indexes...>& indices) const;

  template <int... SigIndexes>
  constexpr Array<T, (sizeof...(SigIndexes)), RestDimensions...> operator[](
      const SigIndices<SigIndexes...>& sig_indices) const;

  template <SliceNotation Notation>
  constexpr auto operator[](const Slice<Notation>& slice) const
      -> Array<T, Slice<Notation>::template ToIndices<FirstDimension>().Size(),
               RestDimensions...>;

  Array operator*(const T scalar)
    requires Multipliable<T>;
  Array operator/(const T scalar)
    requires Divisable<T>;
  Array operator+(const T scalar)
    requires Addable<T>;
  Array operator-(const T scalar)
    requires Subtractable<T>;

  template <typename U>
  Array& operator*=(const U scalar)
    requires Multipliable<T>;
  template <typename U>
  Array& operator/=(const U scalar)
    requires Divisable<T>;
  template <typename U>
  Array& operator+=(const U scalar)
    requires Addable<T>;
  template <typename U>
  Array& operator-=(const U scalar)
    requires Subtractable<T>;

  template <typename U>
  Array<std::common_type_t<T, U>, FirstDimension, RestDimensions...>&
  operator*=(const Array<U, FirstDimension, RestDimensions...>& rhs);
  template <typename U>
  Array<std::common_type_t<T, U>, FirstDimension, RestDimensions...>&
  operator/=(const Array<U, FirstDimension, RestDimensions...>& rhs);
  template <typename U>
  Array<std::common_type_t<T, U>, FirstDimension, RestDimensions...>&
  operator+=(const Array<U, FirstDimension, RestDimensions...>& rhs);
  template <typename U>
  Array<std::common_type_t<T, U>, FirstDimension, RestDimensions...>&
  operator-=(const Array<U, FirstDimension, RestDimensions...>& rhs);

 private:
  template <std::input_iterator IT>
  void InitBuffer(IT begin, IT end, AllocationType alloc_type);

  std::shared_ptr<ArrayData<T>> data_;
  const std::vector<size_t> strides_;
  std::vector<std::vector<size_t>> offsets_;
};

template <typename T, size_t FirstDimension, size_t... RestDimensions>
constexpr Array<T, FirstDimension, RestDimensions...>::Array(
    const Array& other) noexcept
    : data_(other.data_), strides_(other.strides_), offsets_(other.offsets_) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
constexpr Array<T, FirstDimension, RestDimensions...>::Array(
    const Array&& other) noexcept
    : data_(std::move(other->data_)),
      strides_(other.strides_),
      offsets_(std::move(other.offsets_)) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
constexpr Array<T, FirstDimension, RestDimensions...>&
Array<T, FirstDimension, RestDimensions...>::operator=(
    const Array<T, FirstDimension, RestDimensions...>& other) noexcept {
  this->data_ = other->data_;
  this->offsets_ = other.offsets_;
}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
constexpr Array<T, FirstDimension, RestDimensions...>&
Array<T, FirstDimension, RestDimensions...>::operator=(
    Array<T, FirstDimension, RestDimensions...>&& other) noexcept {
  this->data_ = std::move(other);
  this->offsets_ = std::move(other.offsets_);
}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
constexpr Array<T, FirstDimension, RestDimensions...>::Array(
    const AllocationType alloc_type)
  requires std::default_initializable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, domain())),
      strides_(domain::StrideVec()) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <std::input_iterator IT>
constexpr Array<T, FirstDimension, RestDimensions...>::Array(
    IT begin, IT end, const AllocationType alloc_type)
  requires std::copyable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, domain(), begin, end)),
      strides_(domain::StrideVec()) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
constexpr Array<T, FirstDimension, RestDimensions...>::Array(
    std::initializer_list<T> init_list, const AllocationType alloc_type)
  requires std::copyable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, domain(),
                                           init_list.begin(), init_list.end())),
      strides_(domain::StrideVec()) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
constexpr Array<T, FirstDimension, RestDimensions...>::Array(
    const std::vector<T>& from_vec, const AllocationType alloc_type)
  requires std::copyable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, domain(),
                                           from_vec.begin(), from_vec.end())),
      strides_(domain::StrideVec()) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
constexpr Array<T, FirstDimension, RestDimensions...>::Array(
    T&& t, const AllocationType alloc_type)
  requires std::copyable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, domain(),
                                           std::forward<T>(t))),
      strides_(domain::StrideVec()) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <typename FirstTuple, typename... RestTuples>
constexpr Array<T, FirstDimension, RestDimensions...>::Array(
    const AllocationType alloc_type, FirstTuple&& first_elem,
    RestTuples&&... rest_elems)
  requires TupleForT<T, FirstTuple> && (TupleForT<T, RestTuples> && ...)
    : data_(std::make_shared<ArrayData<T>>(
          alloc_type, domain(), std::forward<FirstTuple>(first_elem),
          std::forward<RestTuples>(rest_elems)...)),
      strides_(domain::StrideVec()) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <typename U, typename V>
constexpr Array<T, FirstDimension, RestDimensions...>::Array(
    const MDInitList<T, U, V, FirstDimension, RestDimensions...>& md_init_list,
    const AllocationType alloc_type)
    : data_(std::make_shared<ArrayData<T>>(
          alloc_type, domain(), md_init_list.begin(), md_init_list.end())),
      strides_(domain::StrideVec()) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
constexpr Array<T, FirstDimension, RestDimensions...>::Array(
    const Seq<T, FirstDimension, RestDimensions...>& seq,
    const AllocationType alloc_type)
  requires std::semiregular<T> && std::equality_comparable<T>
    : data_(std::make_shared<ArrayData<T>>(alloc_type, seq)),
      strides_(domain::StrideVec()) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
constexpr Array<T, FirstDimension, RestDimensions...>::Array::Array(
    std::shared_ptr<ArrayData<T>> data, const std::vector<size_t> strides,
    const std::vector<std::vector<size_t>> offsets) noexcept
    : data_(data), strides_(strides), offsets_(std::move(offsets)) {}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <size_t RhsFirstDimension, size_t... RhsRestDimensions>
constexpr Array<T, RhsFirstDimension, RhsRestDimensions...>
Array<T, FirstDimension, RestDimensions...>::Reshape(
    const Domain<RhsFirstDimension, RhsRestDimensions...>& domain_to_shape) {
  static_assert(domain_to_shape.Size() == domain().Size(),
                "Cannot reshape with the given dimension");
  Array<T, RhsFirstDimension, RhsRestDimensions...> reshaped;
  reshaped.data_ = this->data_;
  return reshaped;
}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <typename Self>
constexpr T& Array<T, FirstDimension, RestDimensions...>::operator[](
    this Self&& self, const size_t index)
  requires(sizeof...(RestDimensions) == 0)
{
  size_t data_offset = 0;
  for (size_t i = 0, orig_rank = self.strides_.size();
       i < orig_rank - 1 && i < self.offsets_.size(); ++i) {
    const size_t stride_offset =
        self.offsets_.at(i).empty() ? 0 : self.offsets_.at(i).front();
    data_offset += stride_offset * self.strides_.at(i);
  }
  data_offset += index;
  return self.data_->NthElement(data_offset);
}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
constexpr Array<T, RestDimensions...>
Array<T, FirstDimension, RestDimensions...>::operator[](const size_t index)
  requires(sizeof...(RestDimensions) != 0)
{
  const size_t rank_nth = strides_.size() - domain::Rank();
  std::vector<std::vector<size_t>> subarr_offsets(offsets_);
  while (subarr_offsets.size() <= rank_nth) {
    subarr_offsets.emplace_back();
  }
  if (subarr_offsets.at(rank_nth).empty()) {
    subarr_offsets[rank_nth].emplace_back(index);
  } else {
    const auto& offset_vec = subarr_offsets.at(rank_nth);
    if (offset_vec.size() <= index) {
      throw std::out_of_range("Out of bound");
    }
    const size_t off_index = offset_vec.at(index);
    subarr_offsets[rank_nth].clear();
    subarr_offsets[rank_nth].emplace_back(off_index);
  }

  return {data_, strides_, subarr_offsets};
}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <size_t... Indexes>
constexpr Array<T, (sizeof...(Indexes)), RestDimensions...>
Array<T, FirstDimension, RestDimensions...>::operator[](
    const Indices<Indexes...>& indices) const {
  constexpr size_t max_index = Max<Indexes...>();
  static_assert((max_index < FirstDimension), "Out of range");
  const size_t rank_nth = strides_.size() - domain::Rank();

  if (offsets_.size() > rank_nth && !offsets_.at(rank_nth).empty() &&
      offsets_.at(rank_nth).size() < max_index) {
    throw std::out_of_range("Out of range");
  }

  std::vector<std::vector<size_t>> subarr_offsets(offsets_);
  std::array<size_t, sizeof...(Indexes)> index_arr{Indexes...};
  while (subarr_offsets.size() <= rank_nth) {
    subarr_offsets.emplace_back();
  }

  if (!subarr_offsets.at(rank_nth).empty()) {
    auto& offset_vec = subarr_offsets[rank_nth];
    std::transform(
        index_arr.begin(), index_arr.end(), index_arr.begin(),
        [&offset_vec](const size_t idx) { return offset_vec.at(idx); });
  }
  std::copy(index_arr.begin(), index_arr.end(),
            std::back_inserter(subarr_offsets[rank_nth]));
  return {data_, strides_, subarr_offsets};
}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <int... SigIndexes>
constexpr Array<T, (sizeof...(SigIndexes)), RestDimensions...>
Array<T, FirstDimension, RestDimensions...>::operator[](
    const SigIndices<SigIndexes...>& sig_indices) const {
  return operator[](sig_indices.ToIndices());
}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <SliceNotation Notation>
constexpr auto Array<T, FirstDimension, RestDimensions...>::operator[](
    const Slice<Notation>& slice) const
    -> Array<T, Slice<Notation>::template ToIndices<FirstDimension>().Size(),
             RestDimensions...> {
  return operator[](Slice<Notation>::template ToIndices<FirstDimension>());
}

template <typename T, size_t FirstDimension, size_t... RestDimensions>
template <std::input_iterator IT>
void Array<T, FirstDimension, RestDimensions...>::InitBuffer(
    IT begin, IT end, AllocationType alloc_type) {
  assert(std::distance(begin, end) == domain().size());
  data_ = std::make_shared<ArrayData<T>>(domain(), alloc_type);
  data_->FillIn(begin, end);
}

}  // namespace pp1
#endif