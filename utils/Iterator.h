#ifndef FAITHFUL_ITERATOR_H
#define FAITHFUL_ITERATOR_H

#include <cstddef>
#include <utility>


/// PRECAUTION: this file is only for studying purposes
///  it will be replaced by std::


namespace faithful {
namespace utility {

// __Iterator__ Requirements:
// type_traits: ValueType, Pointer, Reference, DifferenceType, IteratorCategory
// operators: all (depends on IteratorCategory)
// functions:
//  Pointer Current() const;
//  Pointer End() const.

struct InputIteratorTag {};
struct OutputIteratorTag {};
struct ForwardIteratorTag {};
struct BidirectionalIteratorTag {};
struct RandomAccessIteratorTag {};

// TODO: add __PointerTraits__ for case if Iterator has no such type_traits
//  then the only type_traits requirements are ValueType and IteratorCategory
template <typename It>
struct IteratorTraits {
  using ValueType = typename It::ValueType;
  using Pointer = typename It::Pointer;
  using Reference = typename It::Reference;
  using DifferenceType = typename It::DifferenceType;
  using IteratorCategory = typename It::IteratorCategory;
};

template <typename It>
struct IteratorTraits<It*> {
  using ValueType = It;
  using Pointer = It*;
  using Reference = It&;
  using DifferenceType = std::ptrdiff_t;
  using IteratorCategory = RandomAccessIteratorTag;
};

template <typename It>
struct IteratorTraits<const It*> {
  using ValueType = It;
  using Pointer = const It*;
  using Reference = const It&;
  using DifferenceType = std::ptrdiff_t;
  using IteratorCategory = RandomAccessIteratorTag;
};

// Must be at least an InputIterator (Category == InputIteratorTag)
template <typename It,
          typename Category = typename IteratorTraits<It>::IteratorCategory,
          typename =
              std::enable_if<!std::is_same_v<Category, OutputIteratorTag>>>
class MakeConstIterator;

template <typename It>
class MakeConstIterator<It, InputIteratorTag> {
 public:
  using ValueType = typename IteratorTraits<It>::ValueType;
  using Pointer = const typename IteratorTraits<It>::Pointer;
  using Reference = const typename IteratorTraits<It>::Reference;
  using DifferenceType = typename IteratorTraits<It>::DifferenceType;
  using IteratorCategory = InputIteratorTag;

  MakeConstIterator() noexcept(std::is_nothrow_default_constructible_v<It>) =
      default;

  constexpr MakeConstIterator(const MakeConstIterator& other) noexcept =
      default;

  constexpr MakeConstIterator(MakeConstIterator&& other) noexcept = default;

  constexpr MakeConstIterator& operator=(
      const MakeConstIterator& other) noexcept = default;

  constexpr MakeConstIterator& operator=(MakeConstIterator&& other) noexcept =
      default;

  constexpr MakeConstIterator(Pointer current, Pointer end) noexcept(
      std::is_nothrow_constructible_v<It, Pointer, Pointer>)
      : iterator_(current, end) {
  }

  constexpr MakeConstIterator(const It& it) noexcept(
      std::is_nothrow_copy_constructible_v<It>)
      : iterator_(it) {
  }

  constexpr MakeConstIterator(It&& it) noexcept(
      std::is_nothrow_move_constructible_v<It>)
      : iterator_(it) {
  }

  constexpr MakeConstIterator& operator=(const It& it) noexcept(
      std::is_nothrow_copy_assignable_v<It>) {
    iterator_ = std::move(it);
  }

  constexpr MakeConstIterator& operator=(It&& it) noexcept(
      std::is_nothrow_move_assignable_v<It>) {
    iterator_ = std::move(it);
  }

  constexpr MakeConstIterator& operator=(const ValueType& it) noexcept(
      std::is_nothrow_assignable_v<It, ValueType>) {
    iterator_ = it;
    return *this;
  }

  constexpr Reference operator*() const
      noexcept(noexcept(*std::declval<It>())) {
    return *iterator_;
  }

  constexpr Reference operator->() const
      noexcept(noexcept(std::declval<It>().operator->())) {
    return iterator_.operator->();
  }

  constexpr MakeConstIterator& operator++() noexcept(
      noexcept(++std::declval<It&>())) {
    ++iterator_;
    return *this;
  }

  constexpr MakeConstIterator operator++(int) noexcept(
      noexcept(std::declval<It>()++, void())) {
    MakeConstIterator old(*this);
    ++iterator_;
    return old;
  }

  constexpr const It Iterator() const noexcept {
    return iterator_;
  }

  friend constexpr bool operator==(
      const MakeConstIterator& it1,
      const MakeConstIterator& it2) noexcept(noexcept(std::declval<It>() ==
                                                      std::declval<It>())) {
    return it1.iterator_ == it2.iterator_;
  }

  friend constexpr bool operator!=(
      const MakeConstIterator& it1,
      const MakeConstIterator& it2) noexcept(noexcept(std::declval<It>() !=
                                                      std::declval<It>())) {
    return it1.iterator_ != it2.iterator_;
  }

 protected:
  It iterator_;
};

template <typename It>
class MakeConstIterator<It, ForwardIteratorTag>
    : public MakeConstIterator<It, InputIteratorTag> {
 public:
  using Base = MakeConstIterator<It, InputIteratorTag>;
  using ValueType = typename Base::ValueType;
  using Pointer = typename Base::Pointer;
  using Reference = typename Base::Reference;
  using DifferenceType = typename Base::DifferenceType;
  using IteratorCategory = ForwardIteratorTag;

  using Base::Base;

 protected:
  using Base::iterator_;
};

template <typename It>
class MakeConstIterator<It, BidirectionalIteratorTag>
    : public MakeConstIterator<It, ForwardIteratorTag> {
 public:
  using Base = MakeConstIterator<It, ForwardIteratorTag>;
  using ValueType = typename Base::ValueType;
  using Pointer = typename Base::Pointer;
  using Reference = typename Base::Reference;
  using DifferenceType = typename Base::DifferenceType;
  using IteratorCategory = BidirectionalIteratorTag;

  using Base::Base;

  constexpr MakeConstIterator& operator--() noexcept(
      noexcept(--std::declval<It&>())) {
    --iterator_;
    return *this;
  }

  constexpr MakeConstIterator operator--(int) noexcept(
      noexcept(std::declval<It&>()++, void())) {
    MakeConstIterator old(*this);
    --iterator_;
    return old;
  }

 protected:
  using Base::iterator_;
};

template <typename It>
class MakeConstIterator<It, RandomAccessIteratorTag>
    : public MakeConstIterator<It, BidirectionalIteratorTag> {
 public:
  using Base = MakeConstIterator<It, BidirectionalIteratorTag>;
  using ValueType = typename Base::ValueType;
  using Pointer = typename Base::Pointer;
  using Reference = typename Base::Reference;
  using DifferenceType = typename Base::DifferenceType;
  using IteratorCategory = RandomAccessIteratorTag;

  using Base::Base;

  constexpr MakeConstIterator operator+(DifferenceType n) const
      noexcept(noexcept(std::declval<It>() + n)) {
    MakeConstIterator result = *this;
    result += n;
    return result;
  }

  constexpr MakeConstIterator operator-(DifferenceType n) const
      noexcept(noexcept(std::declval<It>() - n)) {
    MakeConstIterator result = *this;
    result -= n;
    return result;
  }

  constexpr MakeConstIterator& operator+=(DifferenceType n) noexcept(
      noexcept(std::declval<It&>() += n)) {
    iterator_ += n;
    return *this;
  }

  constexpr MakeConstIterator& operator-=(DifferenceType n) noexcept(
      noexcept(std::declval<It&>() -= n)) {
    iterator_ -= n;
    return *this;
  }

  constexpr Reference operator[](std::size_t n) const
      noexcept(noexcept(*(std::declval<It>() + n))) {
    return *(iterator_ + n);
  }

  friend constexpr bool operator<(
      const MakeConstIterator& it1,
      const MakeConstIterator& it2) noexcept(noexcept(std::declval<It>() <
                                                      std::declval<It>())) {
    return it1.iterator_ < it2.iterator_;
  }

  friend constexpr bool operator>(
      const MakeConstIterator& it1,
      const MakeConstIterator& it2) noexcept(noexcept(std::declval<It>() >
                                                      std::declval<It>())) {
    return it1.iterator_ > it2.iterator_;
  }

  friend constexpr bool operator<=(
      const MakeConstIterator& it1,
      const MakeConstIterator& it2) noexcept(noexcept(std::declval<It>() <=
                                                      std::declval<It>())) {
    return it1.iterator_ <= it2.iterator_;
  }

  friend constexpr bool operator>=(
      const MakeConstIterator& it1,
      const MakeConstIterator& it2) noexcept(noexcept(std::declval<It>() >=
                                                      std::declval<It>())) {
    return it1.iterator_ >= it2.iterator_;
  }

  friend constexpr DifferenceType operator-(
      const MakeConstIterator& it1,
      const MakeConstIterator& it2) noexcept(noexcept(std::declval<It>() -
                                                      std::declval<It>())) {
    return it1.iterator_ - it2.iterator_;
  }

 protected:
  using Base::iterator_;
};

// Must be at least an BidirectionalIterator
//   (Category == BidirectionalIteratorTag)
template <typename It,
          typename Category = typename IteratorTraits<It>::IteratorCategory,
          typename =
              std::enable_if<!std::is_same_v<Category, OutputIteratorTag> &&
                             !std::is_same_v<Category, InputIteratorTag> &&
                             !std::is_same_v<Category, ForwardIteratorTag>>>
class MakeReverseIterator;

template <typename It>
class MakeReverseIterator<It, BidirectionalIteratorTag> {
 public:
  using ValueType = typename IteratorTraits<It>::ValueType;
  using Pointer = const typename IteratorTraits<It>::Pointer;
  using Reference = const typename IteratorTraits<It>::Reference;
  using DifferenceType = typename IteratorTraits<It>::DifferenceType;
  using IteratorCategory = BidirectionalIteratorTag;

  MakeReverseIterator() noexcept(std::is_nothrow_default_constructible_v<It>) =
      default;

  constexpr MakeReverseIterator(const MakeReverseIterator& other) noexcept =
      default;

  constexpr MakeReverseIterator(MakeReverseIterator&& other) noexcept = default;

  constexpr MakeReverseIterator& operator=(
      const MakeReverseIterator& other) noexcept = default;

  constexpr MakeReverseIterator& operator=(
      MakeReverseIterator&& other) noexcept = default;

  constexpr MakeReverseIterator(Pointer current, Pointer end) noexcept(
      std::is_nothrow_constructible_v<It, Pointer, Pointer>)
      : iterator_(end - 1, current - 1) {
  }

  constexpr MakeReverseIterator(const It& it) noexcept(
      std::is_nothrow_copy_constructible_v<It>)
      : iterator_(it.End() - 1, it.Current() - 1) {
  }

  constexpr MakeReverseIterator(It&& it) noexcept(
      std::is_nothrow_move_constructible_v<It>)
      : iterator_(it.End() - 1, it.Current() - 1) {
  }

  constexpr MakeReverseIterator& operator=(const It& it) noexcept(
      std::is_nothrow_copy_assignable_v<It>) {
    iterator_ = It(it.End() - 1, it.Current() - 1);
  }

  constexpr MakeReverseIterator& operator=(It&& it) noexcept(
      std::is_nothrow_move_assignable_v<It>) {
    iterator_ = It(it.End() - 1, it.Current() - 1);
  }

  constexpr MakeReverseIterator& operator=(const ValueType& it) noexcept(
      std::is_nothrow_assignable_v<It, ValueType>) {
    iterator_ = it;
    return *this;
  }

  constexpr Reference operator*() const
      noexcept(noexcept(*std::declval<It>())) {
    return *iterator_;
  }

  constexpr Reference operator->() const
      noexcept(noexcept(std::declval<It>().operator->())) {
    return iterator_.operator->();
  }

  constexpr MakeReverseIterator& operator++() noexcept(
      noexcept(--std::declval<It&>())) {
    --iterator_;
    return *this;
  }

  constexpr MakeReverseIterator operator++(int) noexcept(
      noexcept(std::declval<It>()--, void())) {
    MakeConstIterator old(*this);
    --iterator_;
    return old;
  }

  constexpr MakeReverseIterator& operator--() noexcept(
      noexcept(++std::declval<It&>())) {
    ++iterator_;
    return *this;
  }

  constexpr MakeReverseIterator operator--(int) noexcept(
      noexcept(std::declval<It>()++, void())) {
    MakeConstIterator old(*this);
    ++iterator_;
    return old;
  }

  friend constexpr bool operator==(
      const MakeReverseIterator& it1,
      const MakeReverseIterator& it2) noexcept(noexcept(std::declval<It>() ==
                                                        std::declval<It>())) {
    return it1.iterator_ == it2.iterator_;
  }

  friend constexpr bool operator!=(
      const MakeReverseIterator& it1,
      const MakeReverseIterator& it2) noexcept(noexcept(std::declval<It>() !=
                                                        std::declval<It>())) {
    return it1.iterator_ != it2.iterator_;
  }

 protected:
  It iterator_;
};

template <typename It>
class MakeReverseIterator<It, RandomAccessIteratorTag>
    : public MakeConstIterator<It, BidirectionalIteratorTag> {
 public:
  using Base = MakeConstIterator<It, BidirectionalIteratorTag>;
  using ValueType = typename Base::ValueType;
  using Pointer = typename Base::Pointer;
  using Reference = typename Base::Reference;
  using DifferenceType = typename Base::DifferenceType;
  using IteratorCategory = RandomAccessIteratorTag;

  constexpr MakeReverseIterator operator+(DifferenceType n) const
      noexcept(noexcept(std::declval<It>() - n)) {
    return iterator_ - n;
  }

  constexpr MakeReverseIterator operator-(DifferenceType n) const
      noexcept(noexcept(std::declval<It>() + n)) {
    return iterator_ + n;
  }

  constexpr MakeReverseIterator operator+=(DifferenceType n) noexcept(
      noexcept(std::declval<It>() -= n)) {
    iterator_ -= n;
    return *this;
  }

  constexpr MakeReverseIterator operator-=(DifferenceType n) noexcept(
      noexcept(std::declval<It>() += n)) {
    iterator_ += n;
    return *this;
  }

  constexpr Reference operator[](std::size_t n) const
      noexcept(noexcept(*(std::declval<It>() - n))) {
    return *(iterator_ - n);
  }

  friend constexpr bool operator<(
      const MakeReverseIterator& it1,
      const MakeReverseIterator& it2) noexcept(noexcept(std::declval<It>() >
                                                        std::declval<It>())) {
    return it1.iterator_ > it2.iterator_;
  }

  friend constexpr bool operator>(
      const MakeReverseIterator& it1,
      const MakeReverseIterator& it2) noexcept(noexcept(std::declval<It>() <
                                                        std::declval<It>())) {
    return it1.iterator_ < it2.iterator_;
  }

  friend constexpr bool operator<=(
      const MakeReverseIterator& it1,
      const MakeReverseIterator& it2) noexcept(noexcept(std::declval<It>() >=
                                                        std::declval<It>())) {
    return it1.iterator_ >= it2.iterator_;
  }

  friend constexpr bool operator>=(
      const MakeReverseIterator& it1,
      const MakeReverseIterator& it2) noexcept(noexcept(std::declval<It>() <=
                                                        std::declval<It>())) {
    return it1.iterator_ <= it2.iterator_;
  }

  friend constexpr DifferenceType operator-(
      const MakeReverseIterator& it1,
      const MakeReverseIterator& it2) noexcept(noexcept(std::declval<It>() -
                                                        std::declval<It>())) {
    return it2.iterator_ - it1.iterator_;
  }

 protected:
  using Base::iterator_;
};

}  // namespace utility
}  // namespace faithful

#endif  // FAITHFUL_ITERATOR_H
