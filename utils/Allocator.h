#ifndef FAITHFUL_ALLOCATOR_H
#define FAITHFUL_ALLOCATOR_H

#include <memory>
#include <set>
#include <type_traits>
#include <unordered_map>

namespace faithful {

namespace details {
namespace allocator {

template <typename T>
struct SizeComparator {
  constexpr bool operator()(
      const std::pair<std::size_t, T>& arg1,
      const std::pair<std::size_t, T>& arg2) const noexcept {
    return arg1.first < arg2.first;
  }
};

template <typename T>
class BaseAllocator {
 public:
  using ValueType = T;
  using Pointer = T*;
  using ConstPointer = const T*;
  using SizeType = std::size_t;

 protected:
  static std::multiset<std::pair<std::size_t, Pointer>,
                       SizeComparator<Pointer>>* buffer_pool_;
};

}  // namespace allocator
}  // namespace details

namespace utility {

// __Allocator__ Requirements:
// type_traits: ValueType, Pointer, ConstPointer, SizeType

// TODO: add __PointerTraits__ for case if Allocator has no such type_traits
//  then the only type_traits requirement is ValueType
template <typename Alloc>
struct AllocatorTraits {
  using AllocatorType = Alloc;
  using ValueType = typename Alloc::ValueType;
  using Pointer = typename Alloc::Pointer;
  using ConstPointer = typename Alloc::ConstPointer;
  using SizeType = typename Alloc::SizeType;

  static Pointer Allocate(AllocatorType& alloc, SizeType size) {
    return alloc.Allocate(size);
  }

  static void Deallocate(AllocatorType& alloc, Pointer ptr, SizeType size) {
    alloc.Deallocate(ptr, size);
  }

  template <typename T, typename... Args>
  static void Construct(AllocatorType& alloc, T* ptr, Args... args) {
    static_assert(std::is_same_v<Pointer, T*>);
    ::new (static_cast<void*>(ptr)) T(std::forward<Args>(args)...);
    // TODO: if not exist (?)
    // alloc.Construct(ptr, args...);
  }

  template <typename U>
  static void Destroy(AllocatorType& alloc, U* ptr) {
    // lloc.Destroy(ptr); // if exist
    if constexpr (std::is_class_v<U>) {
      ptr->~T();
    }
  }
};

// BufferAllocator stands for big rare allocation
// oposite to General-Purpose allocator. because we allocate big blocks
// working in context of __sessions__ where at the end of each session we
//   free() all memory or __transfer__ it to another allocators (e.g. to G-P
//   alloc)
/// We using BufferAllocator for buffers (big blocks of memory), so
///  its includes texture/model/audio/video/fonts/animation loading.
///  Used the most at the beginning

// TODO: class BufferAllocator (storage buffers (256-1024 bytes) + smart
// reusing)
//      set of buffers?? (that is why we've implemented Span comparator)
//      __for_Buffer.h__
// allocate buffers separatedly and reuse them
template <typename T>
class SpanBufferAllocator final : public details::allocator::BaseAllocator<T> {
 public:
  using Base = details::allocator::BaseAllocator<T>;
  using ValueType = typename Base::ValueType;
  using Pointer = typename Base::Pointer;
  using ConstPointer = typename Base::ConstPointer;
  using SizeType = typename Base::SizeType;

  static Pointer Allocate(SizeType size) {
    std::pair<std::size_t, Pointer> temp(size, nullptr);
    auto matched_size = buffer_pool_->lower_bound(temp);
    if (matched_size != buffer_pool_->end()) {
      auto new_buffer = std::move(*matched_size);
      buffer_pool_->erase(matched_size);
      return new_buffer.second;
    }
    auto new_buffer = new T[size];
    return new_buffer;
  }

  static void Deallocate(Pointer ptr, SizeType size) {
    buffer_pool_->emplace(size, ptr);
  }

  template <typename... Args>
  void Construct(T* ptr, Args... args) {
    AllocatorTraits<SpanBufferAllocator<T>>::Construct(*this, ptr,
                                                       std::forward(args)...);
  }

  void Destroy(T* ptr) {
    AllocatorTraits<SpanBufferAllocator<T>>::Destroy(*this, ptr);
  }

 protected:
  using Base::buffer_pool_;
};

template <typename T>
class RawBufferAllocator final : public details::allocator::BaseAllocator<T> {
 public:
  using Base = details::allocator::BaseAllocator<T>;
  using ValueType = typename Base::ValueType;
  using Pointer = typename Base::Pointer;
  using ConstPointer = typename Base::ConstPointer;
  using SizeType = typename Base::SizeType;

  static Pointer Allocate(SizeType size) {
    std::pair<std::size_t, Pointer> temp(size, nullptr);
    auto matched_size = buffer_pool_->lower_bound(temp);
    if (matched_size != buffer_pool_->end()) {
      auto new_buffer = std::move(*matched_size);
      buffer_pool_->erase(matched_size);
      allocated_->insert(std::make_pair(new_buffer.second, new_buffer.first));
      return new_buffer.second;
    }
    auto new_buffer = new T[size];
    allocated_->insert(std::make_pair(new_buffer, size));
    return new_buffer;
  }

  static void Deallocate(Pointer ptr) {
    auto node = allocated_->find(ptr);
    if (node != allocated_->end()) {
      buffer_pool_->insert(std::pair<std::size_t, Pointer>(node->second, ptr));
      allocated_->erase(node);
    }
  }

  template <typename... Args>
  void Construct(T* ptr, Args... args) {
    AllocatorTraits<SpanBufferAllocator<T>>::Construct(*this, ptr,
                                                       std::forward(args)...);
  }

  void Destroy(T* ptr) {
    AllocatorTraits<SpanBufferAllocator<T>>::Destroy(*this, ptr);
  }

 protected:
  using Base::buffer_pool_;
  static std::unordered_map<void*, std::size_t>* allocated_;
};

// TODO: class FunctionAllocator (not big but alligned)
//      __for_Function.h__ (or __for_Task.h__ - I haven't created this file)
// allocate a large block of memory and use it for all functions
// like an allocator from ArtemVladimirov Course of ComplexSystem
template <typename T>
class FunctionAllocator {
 public:
  using ValueType = T;
  using Pointer = T*;
  using ConstPointer = const T*;
  using SizeType = std::size_t;

  Pointer Allocate(SizeType size) {
  }
  void Deallocate(Pointer ptr, SizeType size) {
  }

  template <typename... Args>
  void Construct(T* ptr, Args... args) {
  }
  void Destroy(T* ptr) {
  }

 private:
  std::multiset<std::pair<std::size_t, Pointer>> buffer_pool_;
};

}  // namespace utility
}  // namespace faithful

namespace faithful {

namespace details {
namespace allocator {

template <typename T>
std::multiset<std::pair<std::size_t, typename BaseAllocator<T>::Pointer>,
              SizeComparator<typename BaseAllocator<T>::Pointer>>*
    BaseAllocator<T>::buffer_pool_ = new std::multiset<
        std::pair<std::size_t, typename BaseAllocator<T>::Pointer>,
        SizeComparator<typename BaseAllocator<T>::Pointer>>;

}  // namespace allocator
}  // namespace details

namespace utility {

template <typename T>
std::unordered_map<void*, std::size_t>* RawBufferAllocator<T>::allocated_ =
    new std::unordered_map<void*, std::size_t>;

}  // namespace utility
}  // namespace faithful

#endif  // FAITHFUL_ALLOCATOR_H
