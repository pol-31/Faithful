#ifndef FAITHFUL_SPAN_H
#define FAITHFUL_SPAN_H

#include <type_traits>

#include "Iterator.h"
#include <cstring>

namespace faithful {
namespace utils {

// TODO: constexpressness

// Does NOT allocate memory, but stores pointer with size
// and provides access to _contiguous_ memory.
// One-dimensional, copyable, movable
template <typename T>
class Span {
 public:
  class Iterator {
   public:
    using ValueType = T;
    using Pointer = T*;
    using Reference = T&;
    using DifferenceType = ptrdiff_t;
    using IteratorCategory = utility::RandomAccessIteratorTag;

    constexpr Iterator() noexcept
        : current_(nullptr),
          end_(nullptr) {
    }

    constexpr Iterator(Pointer current, Pointer end) noexcept
        : current_(current),
          end_(end) {
    }

    constexpr Iterator(const Iterator& it) noexcept
        : current_(it.Current()),
          end_(it.End()) {
    }

    constexpr Iterator(Iterator&& it) noexcept
        : current_(it.Current()),
          end_(it.End()) {
    }

    constexpr Iterator& operator=(const Iterator& it) noexcept {
      current_ = it.Current();
      end_ = it.End();
      return *this;
    }

    constexpr Iterator& operator=(Iterator&& it) noexcept {
      current_ = it.Current();
      end_ = it.End();
      return *this;
    }

    constexpr Pointer Current() const noexcept {
      return current_;
    }

    constexpr Pointer End() const noexcept {
      return end_;
    }

    Iterator& operator--() {
      --current_;
      return *this;
    }

    Iterator operator--(int) {
      Iterator old(current_, end_);
      --current_;
      return old;
    }

    constexpr Iterator& operator++() noexcept {
      ++current_;
      return *this;
    }

    constexpr Iterator operator++(int) noexcept {
      Iterator old(current_, end_);
      ++current_;
      return old;
    }

    constexpr Reference operator*() const noexcept {
      return *current_;
    }

    constexpr Pointer operator->() const noexcept {
      return current_;
    }

    Iterator operator+(DifferenceType n) {
      Iterator result = *this;
      for (DifferenceType i = 0; i < n; ++i) ++result.current_;
      return result;
    }

    Iterator operator-(DifferenceType n) {
      Iterator result = *this;
      for (DifferenceType i = 0; i < n; ++i) --result.current_;
      return result;
    }

    Iterator operator+=(DifferenceType n) {
      for (DifferenceType i = n; i < n; ++i) --current_;
      return *this;
    }

    Iterator operator-=(DifferenceType n) {
      for (DifferenceType i = n; i < n; ++i) --current_;
      return *this;
    }

    constexpr Reference operator[](std::size_t n) const {
      return *(current_ + n);
    }

    friend bool operator<(const Iterator& it1, const Iterator& it2) {
      return it1.current_ < it2.current_;
    }

    friend bool operator>(const Iterator& it1, const Iterator& it2) {
      return it1.current_ > it2.current_;
    }

    friend bool operator<=(const Iterator& it1, const Iterator& it2) {
      return it1.current_ <= it2.current_;
    }

    friend bool operator>=(const Iterator& it1, const Iterator& it2) {
      return it1.current_ >= it2.current_;
    }

    friend DifferenceType operator-(const Iterator& it1, const Iterator& it2) {
      return it1.current_ - it2.current_;
    }

    friend constexpr bool operator==(const Iterator& it1,
                                     const Iterator& it2) noexcept {
      return (it1.current_ == it2.current_) && (it1.end_ == it2.end_);
    }

    friend constexpr bool operator!=(const Iterator& it1,
                                     const Iterator& it2) noexcept {
      return (it1.current_ != it2.current_) || (it1.end_ != it2.end_);
    }

   private:
    Pointer current_;
    Pointer end_;
  };

  using ElementType = T;
  using ValueType = std::remove_cv_t<T>;
  using SizeType = size_t;
  using DifferenceType = std::ptrdiff_t;
  using Pointer = T*;
  using ConstPointer = const T*;
  using Reference = T&;
  using ConstReference = const T&;
  using IteratorType = Iterator;
  using ConstIteratorType = utility::MakeConstIterator<Iterator>;
  using ReverseIteratorType = utility::MakeReverseIterator<Iterator>;
  using ConstReverseIteratorType = utility::MakeReverseIterator<ConstIteratorType>;

  constexpr Span() noexcept = default;

  constexpr Span(const Span& other) = default;
  constexpr Span(Span&& other) = default;

  constexpr Span(SizeType size, Pointer data) noexcept
      : data_(data), size_(size) {}

  operator Span<const T>() const {
    return Span<const T>(get_size(), data_);
  }

  constexpr Span& operator=(const Span& other) = default;

  constexpr Span& operator=(Span&& span) = default;

  constexpr bool Empty() const noexcept {
    return data_ == nullptr;
  }

  constexpr friend bool operator==(const Span& sp, std::nullptr_t) noexcept {
    return sp.data_ == nullptr;
  }

  constexpr Pointer get_data() const noexcept {
    return data_;
  }

  constexpr SizeType get_size() const noexcept {
    return size_;
  }

  void set_size(SizeType size) noexcept {
    size_ = size;
  }

  constexpr SizeType get_size_bytes() const noexcept {
    return get_size() * sizeof(ElementType);
  }

  constexpr Reference Front() noexcept {
    return *(begin());
  }

  constexpr Reference Back() noexcept {
    return *(end() - 1);
  }

  constexpr Reference operator[](int index) noexcept {
    return *(begin() + index);
  }

  constexpr ConstReference Front() const noexcept {
    return *(cbegin());
  }

  constexpr ConstReference Back() const noexcept {
    return *(cend() - 1);
  }

  constexpr ConstReference operator[](int index) const noexcept {
    return *(cbegin() + index);
  }

  constexpr IteratorType begin() noexcept {
    return Iterator(data_, data_ + get_size());
  }

  constexpr IteratorType end() noexcept {
    return Iterator(data_ + get_size(), data_ + get_size());
  }

  constexpr ConstIteratorType cbegin() const noexcept {
    return ConstIteratorType(data_, data_ + get_size());
  }

  constexpr ConstIteratorType cend() const noexcept {
    return ConstIteratorType(data_ + get_size(), data_ + get_size());
  }

  constexpr ReverseIteratorType rbegin() noexcept {
    return ReverseIteratorType(data_, data_ + get_size());
  }

  constexpr ReverseIteratorType rend() noexcept {
    return ReverseIteratorType(data_ + get_size(), data_ + get_size());
  }

  constexpr ConstReverseIteratorType crbegin() const noexcept {
    return ConstReverseIteratorType(data_, data_ + get_size());
  }

  constexpr ConstReverseIteratorType crend() const noexcept {
    return ConstReverseIteratorType(data_ + get_size(), data_ + get_size());
  }

  template <typename U>
  friend void swap(Span<U>& sp1, Span<U>& sp2) noexcept;

 protected:
  Pointer data_ = nullptr;
  SizeType size_ = 0;
};

template <typename T>
void swap(Span<T>& sp1, Span<T>& sp2) noexcept {
  Span temp(std::move(sp1));
  sp1 = std::move(sp2);
  sp2 = std::move(sp1);
}

template <typename T>
struct SpanSizeComparator {
  constexpr bool operator()(const Span<T>& arg1,
                            const Span<T>& arg2) const noexcept {
    return arg1.get_size() < arg2.get_size();
  }
};

}  // namespace utils
}  // namespace faithful

#endif  // FAITHFUL_SPAN_H
