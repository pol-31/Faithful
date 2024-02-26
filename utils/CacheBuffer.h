#ifndef FAITHFUL_SRC_LOADER_CACHEBUFFER_H_
#define FAITHFUL_SRC_LOADER_CACHEBUFFER_H_

#include <array>

namespace faithful {
namespace utils {

template <typename T, int size>
class CachedBuffer {
 public:
  static_assert(size >= 0, "size must be non-negative");

  using DataType = std::array<T, size>;
  using ElementType = T;
  using SizeType = typename std::array<T, size>::size_type;

  CachedBuffer() = default;

  void Add(ElementType entry) {
    if (size == 0) {
      return;
    }
    if (next_cache_pos_ < size) {
      data_[next_cache_pos_] = entry;
    } else {
      next_cache_pos_ = 0;
      data_[next_cache_pos_] = entry;
    }
    ++next_cache_pos_;
  }

  void RenewEntry(int i) {
    //TODO:
    // if already last - do nothing
    // else move it to last position
    Add(data_[i]);
  }

  void Clear() {
    data_.fill({});
  }

  SizeType Size() {
    return data_.size();
  }

  ElementType operator[](int idx) {
    return data_[idx];
  }

 private:
  std::array<T, size> data_;
  int next_cache_pos_ = 0;
};

} // namespace utils
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_CACHEBUFFER_H_
