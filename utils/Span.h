#ifndef FAITHFUL_SPAN_H
#define FAITHFUL_SPAN_H

#include <type_traits>

#include "Iterator.h"
#include "Allocator.h"
#include <cstring>

namespace faithful {

namespace details {
namespace span {

// TODO: constexpressness

// SpanCleanerSuitable checks is:
// 1) T static;
// 2) takes pointer as a parameter;
// 3) returns void.
template<typename T, typename = void>
struct SpanAllocatorSuitable : std::false_type {};

template<typename T>
struct SpanAllocatorSuitable<T,
  std::conjunction<std::is_same<
  typename T::ValueType, decltype(T::Allocate(0))>,
  std::void_t<decltype(T::Deallocate(nullptr, 0))>>> : std::true_type {
};

} // namespace span
} // namespace details


namespace utility {

// Does NOT allocate memory, but stores pointer with size
// and provides access to _contiguous_ memory.
// One-dimensional, copyable, movable
// Cleaner type - struct with _static_void_Clean() method,
// which is optional and do nothing by default (LazySpanCleaner)
template<
  typename T,
  typename UserAllocator = faithful::utility::SpanBufferAllocator<T>,
  typename Allocator = std::conditional_t<details::span::SpanAllocatorSuitable<
    UserAllocator>::value, UserAllocator,
    faithful::utility::SpanBufferAllocator<T>>,
  bool Nx = noexcept(Allocator::Allocate(0))>
class Span {
 public:
  class Iterator {
   public:
    using ValueType = T;
    using Pointer = T *;
    using Reference = T &;
    using DifferenceType = ptrdiff_t;
    using IteratorCategory = RandomAccessIteratorTag;

    constexpr Iterator() noexcept: current_(nullptr), end_(nullptr) {}

    constexpr Iterator(Pointer current, Pointer end) noexcept
      : current_(current), end_(end) {}

    constexpr Iterator(const Iterator &it) noexcept
      : current_(it.Current()), end_(it.End()) {}

    constexpr Iterator(Iterator &&it) noexcept
      : current_(it.Current()), end_(it.End()) {}

    constexpr Iterator &operator=(const Iterator &it) noexcept {
      current_ = it.Current();
      end_ = it.End();
      return *this;
    }

    constexpr Iterator &operator=(Iterator &&it) noexcept {
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

    Iterator &operator--() {
      --current_;
      return *this;
    }

    Iterator operator--(int) {
      Iterator old(current_, end_);
      --current_;
      return old;
    }

    constexpr Iterator &operator++() noexcept {
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
      for (DifferenceType i = 0; i < n; ++i)
        ++result.current_;
      return result;
    }

    Iterator operator-(DifferenceType n) {
      Iterator result = *this;
      for (DifferenceType i = 0; i < n; ++i)
        --result.current_;
      return result;
    }

    Iterator operator+=(DifferenceType n) {
      for (DifferenceType i = n; i < n; ++i)
        --current_;
      return *this;
    }

    Iterator operator-=(DifferenceType n) {
      for (DifferenceType i = n; i < n; ++i)
        --current_;
      return *this;
    }

    constexpr Reference operator[](std::size_t n) const {
      return *(current_ + n);
    }

    friend bool operator<(const Iterator &it1, const Iterator &it2) {
      return it1.current_ < it2.current_;
    }

    friend bool operator>(const Iterator &it1, const Iterator &it2) {
      return it1.current_ > it2.current_;
    }

    friend bool operator<=(const Iterator &it1, const Iterator &it2) {
      return it1.current_ <= it2.current_;
    }

    friend bool operator>=(const Iterator &it1, const Iterator &it2) {
      return it1.current_ >= it2.current_;
    }

    friend DifferenceType operator-(const Iterator &it1,
        const Iterator &it2) {
      return it1.current_ - it2.current_;
    }

    friend constexpr bool operator==(const Iterator &it1,
        const Iterator &it2) noexcept {
      return (it1.current_ == it2.current_) &&
             (it1.end_ == it2.end_);
    }

    friend constexpr bool operator!=(const Iterator &it1,
        const Iterator &it2) noexcept {
      return (it1.current_ != it2.current_) ||
             (it1.end_ != it2.end_);
    }

   private:
    Pointer current_;
    Pointer end_;
  };

  // in move-ctor and move-assignment we don't need Clear() for r-object
  // so LazyTag states "there is no deleting"
  struct LazyTag {
  };

  using ElementType = T;
  using ValueType = std::remove_cv_t<T>;
  using SizeType = size_t;
  using DifferenceType = std::ptrdiff_t;
  using Pointer = T *;
  using ConstPointer = const T *;
  using Reference = T &;
  using ConstReference = const T &;
  using IteratorType = Iterator;
  using ConstIteratorType = MakeConstIterator<Iterator>;
  using ReverseIteratorType = MakeReverseIterator<Iterator>;
  using ConstReverseIteratorType = MakeReverseIterator<ConstIteratorType>;

  constexpr Span() noexcept = default;

  Span(const Span& other)
    : data_(other.get_data()), size_and_owner_(other.get_size_and_owner()) {}

  constexpr Span(Span &&other) noexcept
      : data_(other.get_data()), size_and_owner_(other.get_size_and_owner()) {
    other.set_owner(false);
  }

  constexpr Span(SizeType size, Pointer data, bool new_owner = true) noexcept
      : data_(data) {
    set_size(size);
    set_owner(new_owner);
  }

  Span(SizeType size) {
    if (size == 0) return;
    Allocator allocator;
    data_ = allocator.Allocate(size);
    set_owner(true);
    set_size(size);
  }

  operator Span<const T>() const {
    return Span<const T>(get_size(), data_);
  }

  constexpr Span(nullptr_t) noexcept {}

  Span &operator=(const Span& other) {
    Clear();
    data_ = other.get_data();
    set_size(other.get_size());
  }

  constexpr Span &operator=(Span &&span) noexcept(Nx) {
    if (data_ == span.get_data()) {
      [[unlikely]]
      return *this;
    }
    Clear();
    data_ = span.get_data();
    size_and_owner_ = span.get_size_and_owner();
    span.set_owner(false);
    return *this;
  }

  constexpr Span &operator=(nullptr_t) {
    Clear();
  }

  ~Span() noexcept(Nx) {
    Clear();
  }

  constexpr void Clear() noexcept(Nx) {
    if (get_owner()) {
      Allocator allocator;
      allocator.Deallocate(data_, get_size());
    }
    data_ = nullptr;
    size_and_owner_ = 0;
  }

  constexpr bool Empty() const noexcept {
    return data_ == nullptr;
  }

  Span MakeCopy() {
    if (data_ == nullptr)
      return Span{};
    auto size = get_size();
    auto copy = Span(size);
    std::memcpy(copy.get_data(), data_, size);
    return copy;
  }

  constexpr friend bool operator==(const Span& sp, nullptr_t) noexcept {
    return sp.data_ == nullptr;
  }


  constexpr Pointer get_data() const noexcept {
    return data_;
  }

  constexpr bool get_owner() const noexcept {
    return size_and_owner_ & 1U;
  }

  void set_owner(bool owner) {
    size_and_owner_ = (size_and_owner_ & ~1U) | (owner ? 1 : 0);
  }

  constexpr SizeType get_size() const noexcept {
    return size_and_owner_ >> 1;
  }

  void set_size(SizeType size) noexcept {
    size_and_owner_ = (size << 1) | (size_and_owner_ & 1U);
  }

  constexpr SizeType get_size_and_owner() const noexcept {
    return size_and_owner_;
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

  template<typename U, typename V>
  friend void swap(Span<U, V> &sp1, Span<U, V> &sp2) noexcept;

 protected:
  Pointer data_ = nullptr;
  // first (smaller) bit for owner/not_owner
  // this is for the reducing memory usage - _16_bytes_ instead of 17
  SizeType size_and_owner_ = 0;
};

template<typename U, typename V>
void swap(Span<U, V> &sp1, Span<U, V> &sp2) noexcept {
  Span temp(std::move(sp1));
  sp1 = std::move(sp2);
  sp2 = std::move(sp1);
}

template<typename T>
struct SpanSizeComparator {
  constexpr bool operator()(const Span<T>& arg1,
      const Span<T>& arg2) const noexcept {
    return arg1.get_size() < arg2.get_size();
  }
};

} // namespace utility
} // namespace faithful

#endif // FAITHFUL_SPAN_H
