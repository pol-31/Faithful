#ifndef FAITHFUL_SRC_EXECUTORS_QUEUES_QUEUE_H_
#define FAITHFUL_SRC_EXECUTORS_QUEUES_QUEUE_H_

#include <queue>

#include <../../../utils/Function.h>

namespace faithful {
namespace details {
namespace queue {

template <typename Task = folly::Function<void()>>
class IQueueBase {
 public:
  IQueueBase() = default;
  virtual ~IQueueBase() = default;

  virtual Task Front() = 0;

  virtual void Pop() = 0;
  virtual void Push(Task&&) = 0;


  /// we don't use lock_guard, cause we don't need reliability there, this
  /// function should be used only in previous checking for loop optimization
  bool Empty() {
    return task_queue_.empty();
  }

 protected:
  std::queue<Task> task_queue_;
};


} // namespace queue
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_QUEUES_QUEUE_H_
