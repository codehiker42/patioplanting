#ifndef _PP1_ARRAYDATA_H_
#define _PP1_ARRAYDATA_H_

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
  ArrayData(const ArrayData&) = delete;
  ArrayData& operator=(const ArrayData&) = delete;
  ArrayData(ArrayData&& rhs);
  ~ArrayData();

 private:
  T* buffer_{nullptr};
};

template <typename T>
ArrayData<T>::ArrayData() = default;

template <typename T>
ArrayData<T>::~ArrayData() {
  if (buffer_) {
    std::free(buffer_);
  }
}

#endif