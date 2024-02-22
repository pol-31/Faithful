#ifndef FAITHFUL_EXECUTOR_H
#define FAITHFUL_EXECUTOR_H

#include <thread>

#include "queues/IQueue.h"

#include "../../utils/Function.h"

namespace faithful {
namespace details {

class IExecutor {
 public:
  using TaskType = folly::Function<void()>;

  enum class State {
    kNotStarted,
    kRunning,
    kSuspended, // for debug only
    kJoined,
    kTerminated
  };

  IExecutor() = default;
  virtual ~IExecutor() = default;

  /// not copyable
  IExecutor(const IExecutor&) = delete;
  IExecutor& operator=(const IExecutor&) = delete;

  /// movable
  IExecutor(IExecutor&&) = default;
  IExecutor& operator=(IExecutor&&) = default;

  virtual void Run() = 0;
  virtual void Join() = 0;

  TaskType&& Take() {
    if (state_ != State::kJoined &&
        state_ != State::kTerminated) {
      return task_queue_->Pop();
    }
  }

  void Put(TaskType&& task) {
    if (state_ != State::kJoined &&
        state_ != State::kTerminated) {
      task_queue_->Push(std::move(task));
    }
  }

 protected:
  queue::IQueueBase<TaskType>* task_queue_;
  State state_;
};

/// has fixed number of threads
template <int thread_count>
class IStaticExecutor : public IExecutor {
 public:
  IStaticExecutor() = default;

  /// not copyable
  IStaticExecutor(const IStaticExecutor&) = delete;
  IStaticExecutor& operator=(const IStaticExecutor&) = delete;

  /// movable
  IStaticExecutor(IStaticExecutor&&) = default;
  IStaticExecutor& operator=(IStaticExecutor&&) = default;

 protected:
  std::array<std::thread, thread_count> threads_;
};

/// dynamic number of threads (for I/O bounding operations)
class IDynamicExecutor : public IExecutor {
 public:
  IDynamicExecutor() = default;

  /// not copyable
  IDynamicExecutor(const IDynamicExecutor&) = delete;
  IDynamicExecutor& operator=(const IDynamicExecutor&) = delete;

  /// movable
  IDynamicExecutor(IDynamicExecutor&&) = default;
  IDynamicExecutor& operator=(IDynamicExecutor&&) = default;

  virtual void Spawn(int num) = 0;
  virtual void Kill(int num) = 0;

 protected:
  std::vector<std::thread> threads_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_EXECUTOR_H
