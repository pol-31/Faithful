#ifndef FAITHFUL_BUFFER_H
#define FAITHFUL_BUFFER_H

#include <atomic>
#include <string>

#include "Allocator.h"
#include "Span.h"

namespace faithful {
namespace utility {

/// thread-unsafe
/// in case of overflow - the least data lost
template <typename T, typename Alloc = SpanBufferAllocator<T>>
class SpanBuffer {
 public:
  template <typename... Args>
  SpanBuffer(Args&&... args, std::size_t n = 64) {
    primary_storage_ = Span(n, allocator_.Allocate(n));
    if (primary_storage_.get_data()) {
      global_pos_ = primary_storage_.begin();
      for (size_t i = 0; i < n; ++i)
        allocator_.Construct(primary_storage_.get_data() + i,
                             std::forward<Args>(args)...);
    }
  }

  ~SpanBuffer() {
    if (primary_storage_.get_data()) {
      for (size_t i = 0; i < primary_storage_.get_size(); ++i)
        allocator_.Destroy(primary_storage_.get_data() + i);
      allocator_.Deallocate(primary_storage_.get_data(),
                            primary_storage_.get_size());
    }
  }

  void Write(const Span<T> str) {
    if (Full())
      return;
    for (auto i = str.get_data(); i < str.get_data() + str.get_size(); ++i) {
      *global_pos_ = *i;
      if (++global_pos_ == primary_storage_.end())
        return;
    }
  };

  void Write(const std::basic_string<T>& str) {
    if (Full())
      return;
    int free_space = primary_storage_.end() - global_pos_;
    int write_size = std::min(static_cast<int>(str.length()), free_space);
    for (int i = 0; i < write_size; ++i) *global_pos_ = str[i];
  };

  void Write(std::basic_string<T>&& str) {
    if (Full())
      return;
    int free_space = primary_storage_.end() - global_pos_;
    int write_size = std::min(static_cast<int>(str.length()), free_space);
    std::move(str.begin(), str.begin() + write_size, global_pos_);
    global_pos_ += write_size;
  };

  void Write(const T* str, std::size_t str_size = -1) {
    if (Full())
      return;
    if (str_size <= -1)
      for (auto i = str; *i != '\0'; ++i) ++str_size;
    for (auto s = str; str_size > 0; ++s, --str_size) {
      *global_pos_ = *s;
      if (++global_pos_ == primary_storage_.end())
        return;
    }
  };

  void Write(const T ch) {
    if (Full())
      return;
    *global_pos_ = ch;
    ++global_pos_;
  };

  T* Data() {
    return primary_storage_.get_data();
  };

  void Clear() {
    global_pos_ = primary_storage_.begin();
  };

  size_t Size() const {
    return primary_storage_.get_size();
  };

  bool Full() const {
    return (primary_storage_.cend() - global_pos_) == 0;
  };

 protected:
  Span<T> primary_storage_;

 private:
  typename Span<T>::Iterator global_pos_;
  Alloc allocator_;
};

/// thread-safe
/// for more than 10 messages - any next will be lost
template <typename T, typename Alloc = SpanBufferAllocator<T>>
class SpanBufferPool {
 public:
  using BufferType = SpanBuffer<T, Alloc>;
  using DataType = Span<BufferType>;
  using ReadType = DataType;

  SpanBufferPool(std::size_t messages_num = 10) {
    if (messages_num <= 0)
      messages_num = 10;
    records_max_ = messages_num;
    CreateStorage(primary_storage_);
    records_left_.store(messages_num);
  }

  void Write(const Span<T> str) {
    int write_idx = records_left_.fetch_sub(1, std::memory_order_relaxed);
    if (write_idx > 0)
      primary_storage_[records_max_ - records_left_].Write(str);
  };

  void Write(const std::string& str) {
    int write_idx = records_left_.fetch_sub(1, std::memory_order_relaxed);
    if (write_idx > 0)
      primary_storage_[records_max_ - records_left_].Write(str);
  };

  void Write(std::string&& str) {
    int write_idx = records_left_.fetch_sub(1, std::memory_order_relaxed);
    if (write_idx > 0)
      primary_storage_[records_max_ - records_left_].Write(str);
  };

  void Write(SpanBuffer<T> buffer) {
    int write_idx = records_left_.fetch_sub(1, std::memory_order_relaxed);
    if (write_idx > 0)
      primary_storage_[records_max_ - records_left_].Write(buffer.Data(),
                                                           buffer.Size());
  };

  DataType Read() {
    auto obtained_data = std::move(primary_storage_);
    CreateStorage(primary_storage_);
    Clear();
    return std::move(obtained_data);
  };

  void CreateStorage(DataType& storage) {
    storage = DataType(records_max_);  // TODO: Alloc should handle it
    for (auto msg : storage)
      *(msg.Data()) = 0;  // char 0 means Null accordingly to ASCII
  }

  void Clear() {
    records_left_.store(records_max_);
  };

  size_t Size() const {
    return records_max_ - records_left_;
  };

  bool Full() const {
    return records_left_.load(std::memory_order_relaxed) <= 0;
  };

 protected:
  DataType primary_storage_;
  int records_max_;
  std::atomic<int> records_left_;
};

/// thread-safe
/// for more than 2 * 10 messages (10 messages for each storage)
///   - any next will be lost
template <typename T, typename Alloc = SpanBufferAllocator<T>>
class SwitchSpanBufferPool : public SpanBufferPool<T, Alloc> {
 public:
  using Base = SpanBufferPool<T, Alloc>;
  using BufferType = typename Base::BufferType;
  using DataType = typename Base::DataType;
  using ReadType = std::pair<DataType, DataType>;

  SwitchSpanBufferPool(std::size_t messages_num = 10)
      : SpanBufferPool<T, Alloc>(messages_num) {
    Base::CreateStorage(secondary_storage_);
  }

  void Write(const Span<T> str) {
    Switch();
    Base::Write(str);
  };

  void Write(const std::string& str) {
    Switch();
    Base::Write(str);
  };

  void Write(std::string&& str) {
    Switch();
    Base::Write(str);
  };

  void Write(SpanBuffer<T> buffer) {
    Switch();
    Base::Write(buffer);
  };

  ReadType Read() {
    if (secondary_storage_full_.test_and_set()) {
      secondary_storage_full_.clear();
      return {Base::Read(), {}};
    }
    auto obtained_data_1 = std::move(secondary_storage_);
    auto obtained_data_2 = std::move(primary_storage_);
    Base::CreateStorage(primary_storage_);
    Base::CreateStorage(secondary_storage_);
    Clear();
    return {std::move(obtained_data_1), std::move(obtained_data_2)};
  };

  void Clear() {
    secondary_storage_full_.clear();
    Base::Clear();
  };

  bool Full() const {
    return secondary_storage_full_.test_and_set() && Base::Full();
  };

 private:
  void Switch() {
    if (Base::Full() && !secondary_storage_full_.test_and_set()) {
      std::swap(secondary_storage_, primary_storage_);
      records_left_.store(records_max_);
    }
  }

  using Base::primary_storage_;
  using Base::records_left_;
  using Base::records_max_;
  DataType secondary_storage_;
  mutable std::atomic_flag secondary_storage_full_ = ATOMIC_FLAG_INIT;
};

}  // namespace utility
}  // namespace faithful

#endif  // FAITHFUL_BUFFER_H
