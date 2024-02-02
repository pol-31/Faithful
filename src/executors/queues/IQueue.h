#ifndef FAITHFUL_SRC_EXECUTORS_QUEUES_QUEUE_H_
#define FAITHFUL_SRC_EXECUTORS_QUEUES_QUEUE_H_

#include <queue>

#include <../../../utils/Function.h>

namespace faithful {
namespace details {
namespace queue {

template <typename Task>
class IQueueBase {
 public:
  IQueueBase() = default;

  virtual void Front() = 0;

  virtual void Pop() = 0;
  virtual void Push(Task&&) = 0;
 protected:
  std::queue<Task> task_queue_;
};


} // namespace queue
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_QUEUES_QUEUE_H_
