#ifndef FAITHFUL_UTILS_CONSTEXPRVECTOR_H_
#define FAITHFUL_UTILS_CONSTEXPRVECTOR_H_

#include <array>
#include <iostream>

namespace faithful {
namespace utils {

/// std::vector - like interface for std::array
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
  using ConstReverseIterator = typename std::array<T, count>::const_reverse_iterator ;


  // TODO: what if std::move and element already exist in container
  void Push(T element) {
    if (last_element_idx + 1 == data_.size()) {
      // throw exception?
      std::cerr << "Unable to push into ConstexprVector: size overflow"
                << std::endl;
    }
    ++last_element_idx;
    data_[last_element_idx] = element;
  }

  Reference operator[](int idx) {
    return data_[idx];
  }

  SizeType Size() {
    return last_element_idx + 1;
  }

  Reference Back() {
    return data_[last_element_idx];
  }

  void Pop

  /// TODO: implement other std::vector & std::array functions, iterator

 private:
  std::array<T, count> data_;
  int last_element_idx = 0;
};
}
}

#endif  // FAITHFUL_UTILS_CONSTEXPRVECTOR_H_
