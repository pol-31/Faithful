#ifndef FAITHFUL_UTILS_CONSTEXPRVECTOR_H_
#define FAITHFUL_UTILS_CONSTEXPRVECTOR_H_

#include <array>
#include <iostream>
#include <stdexcept>

namespace faithful {
namespace utils {

/// ConstVector - wrapper for std::array - with interface of std::vector
/// 1) We don't care about possible "pushing already existing element
/// to the same container" and similar cases.
/// 2) only useful methods (in Faithful we don't need all vector/array methods)
template <typename T, int count>
class ConstexprVector {
 public:
  using ValueType = typename std::array<T, count>::value_type;
  using SizeType = typename std::array<T, count>::size_type ;
  using DifferenceType = typename std::array<T, count>::difference_type;
  using Reference = typename std::array<T, count>::reference;
  using ConstReference = typename std::array<T, count>::const_reference;
  using Pointer = typename std::array<T, count>::pointer;
  using ConstPointer = typename std::array<T, count>::const_pointer;
  using Iterator = typename std::array<T, count>::iterator;
  using ConstIterator = typename std::array<T, count>::const_iterator;
  using ReverseIterator = typename std::array<T, count>::reverse_iterator;
  using ConstReverseIterator = typename std::array<T, count>::const_reverse_iterator;

  /// data members handle it on their own
  ConstexprVector() = default;
  ConstexprVector(const ConstexprVector&) = default;
  ConstexprVector(ConstexprVector&&) = default;
  ConstexprVector& operator=(const ConstexprVector&) = default;
  ConstexprVector& operator=(ConstexprVector&&) = default;

  /// accessors

  constexpr Reference At(int idx) {
    data_.at(idx);
  }
  constexpr ConstReference At(int idx) const {
    data_.at(idx);
  }

  constexpr Reference operator[](int idx) {
    return data_[idx];
  }
  constexpr ConstReference operator[](int idx) const {
    return data_[idx];
  }

  constexpr Reference Front() {
    return data_[0];
  }
  constexpr ConstReference Front() const {
    return data_[0];
  }

  constexpr Reference Back() {
    return data_[last_element_idx];
  }
  constexpr ConstReference Back() const {
    return data_[last_element_idx];
  }

  constexpr Pointer Data() noexcept {
    return data_.data();
  }
  constexpr ConstPointer Data() const {
    return data_.data();
  }

  /// iterators

  constexpr Iterator Begin() {
    return data_.begin();
  }
  constexpr ConstIterator CBegin() const {
    return data_.cbegin();
  }

  constexpr Iterator End() {
    return data_[last_element_idx + 1];
  }
  constexpr ConstIterator CEnd() const {
    return data_[last_element_idx + 1];
  }

  constexpr ReverseIterator RBegin() {
    return data_[last_element_idx];
  }
  constexpr ConstReverseIterator CRBegin() const {
    return data_[last_element_idx];
  }

  constexpr ReverseIterator REnd() {
    return data_.rend();
  }
  constexpr ConstReverseIterator CREnd() const {
    return data_.crend();
  }

  /// capacity

  constexpr SizeType Capacity() const noexcept {
    return count;
  }

  constexpr bool Empty() const noexcept {
    return last_element_idx == 0;
  }

  constexpr SizeType Size() const noexcept {
    return last_element_idx + 1;
  }

  /// modifiers

  template <typename ... Args>
  constexpr Reference EmplaceBack(Args&&... args) {
    if (last_element_idx + 1 == data_.size()) {
      throw std::out_of_range("ConstexprVector: size overflow");
    } else {
      data_[last_element_idx + 1] = ValueType(std::forward<Args>(args)...);
      ++last_element_idx;
      return data_[last_element_idx];
    }
  }

  template <typename U>
  constexpr void PushBack(U&& element) {
    if (last_element_idx + 1 == data_.size()) {
      throw std::out_of_range("ConstexprVector: size overflow");
    }
    ++last_element_idx;
    data_[last_element_idx] = element;
  }

  constexpr void PopBack() {
    if (last_element_idx != 0) {
      --last_element_idx;
    }
  }

  constexpr void Clear() noexcept {
    last_element_idx = 0;
  }

  constexpr void Fill(const T& value) {
    data_.fill(value);
  }

  constexpr void Swap(ConstexprVector& other) noexcept {
    std::swap(data_, other.data_);
  }

 private:
  std::array<T, count> data_;
  int last_element_idx = 0;
};

} // namespace utils
} // namespace faithful

#endif  // FAITHFUL_UTILS_CONSTEXPRVECTOR_H_
