#ifndef FAITHFUL_SRC_EXECUTORS_QUEUES_LIFOUNBOUNDEDSPSCBLOCKINGQUEUE_H_
#define FAITHFUL_SRC_EXECUTORS_QUEUES_LIFOUNBOUNDEDSPSCBLOCKINGQUEUE_H_

#include <mutex>
#include <condition_variable>

#include "IQueue.h"

namespace faithful {
namespace details {
namespace queue {

/// Usage: AudioThreadPool (wrap OpenAL context)
/// --- --- ---
/// Explanation:
/// ---
/// description:
///   - order of the sounds matters
/// solution:
///   - Lifo (last-in-first-out)
/// ---
/// description:
///   - if some sounds were lagged - just skip them, wait for new
/// solution:
///   - Bounded
/// ---
/// description:
///   - we manage only one OpenAL context, so there's only one consumer;
///     producers are not restricted and can push task from any thread:
/// solution:
///   - MPSC (Multi-Producer-Single-Consumer)
/// ---
/// description:
///   - may be multiple producers from different threads at the same time
/// solution:
///   - Blocking

// TODO: lock-free
template <typename Task = folly::Function<void()>>
class LifoBoundedMPSCBlockingQueue : public IQueueBase<Task> {
 public:
  using Base = IQueueBase<Task>;

  LifoBoundedMPSCBlockingQueue() = default;

  void Pop() override {
    std::unique_lock guard(mutex_);
    queue_busy_.wait(guard, [queue = &task_queue_]() {
      return queue->empty();
    });
    task_queue_.pop();
  }

  void Push(Task&& task) override {
    {
      std::lock_guard guard(mutex_);
      task_queue_.push(std::move(task));
    }
    std::cerr << "notifying" << std::endl;
    queue_busy_.notify_one();
  }

  Task Front() override {
    std::unique_lock guard(mutex_);
    if (task_queue_.empty()) {
      std::cout << "wait again" << std::endl;
      queue_busy_.wait(guard, [queue = &task_queue_]() {
        return queue->empty();
      });
    }
    auto task = std::move(task_queue_.front());
    task_queue_.pop();
//    std::cout << "size " << task_queue_.size() << std::endl;
    return std::move(task);
  }

 private:
  using Base::task_queue_;
  std::mutex mutex_;
  std::condition_variable queue_busy_;
};


} // namespace queue
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_QUEUES_LIFOUNBOUNDEDSPSCBLOCKINGQUEUE_H_
