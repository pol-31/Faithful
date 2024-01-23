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
  // TODO: usings for element/pointer/reference/etc...

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

  T& operator[](int idx) {
    return data_[idx];
  }

  /// TODO: implement other std::vector & std::array functions, iterator

 private:
  std::array<T, count> data_;
  int last_element_idx = 0;
};
}
}

#endif  // FAITHFUL_UTILS_CONSTEXPRVECTOR_H_
