#ifndef FAITHFUL_EXECUTOR_H
#define FAITHFUL_EXECUTOR_H

#include <thread>

#include "queues/IQueue.h"

#include "../../utils/Function.h"

namespace faithful {
namespace details {

class Executor {
 public:
  using Task = folly::Function<void>;

  enum class State {
    kNotStarted,
    kRunning,
    kSuspended,
    kJoined,
    kTerminated
  };

  Executor() = default;

  /// not copyable
  Executor(const Executor&) = delete;
  Executor& operator=(const Executor&) = delete;

  /// not movable
  Executor(Executor&&) = default;
  Executor& operator=(Executor&&) = default;

  virtual void Run() = 0;

  void Pop() {
    task_queue_->Pop();
  }

  void Push(Task&& task) {
    task_queue_->Push(std::move(task));
  }

 protected:
  queue::IQueueBase<Task>* task_queue_;
  State state_;
  // threads / fibers
};

/// has fixed number of threads todo ______________
template <int thread_count>
class StaticExecutor : public Executor {
 public:
  StaticExecutor() = default;

  /// not copyable
  StaticExecutor(const StaticExecutor&) = delete;
  StaticExecutor& operator=(const StaticExecutor&) = delete;

  /// not movable
  StaticExecutor(StaticExecutor&&) = default;
  StaticExecutor& operator=(StaticExecutor&&) = default;

 private:
  std::array<std::thread, thread_count> threads_;
};

/// dynamic number of threads (for I/O loading operations) todo ______________
class DynamicExecutor : public Executor {
 public:
  DynamicExecutor() = default;

  /// not copyable
  DynamicExecutor(const DynamicExecutor&) = delete;
  DynamicExecutor& operator=(const DynamicExecutor&) = delete;

  /// not movable
  DynamicExecutor(DynamicExecutor&&) = default;
  DynamicExecutor& operator=(DynamicExecutor&&) = default;

 private:
  std::vector<std::thread> threads_;
};

} // namespace faithful
} // namespace details


#endif  // FAITHFUL_EXECUTOR_H
