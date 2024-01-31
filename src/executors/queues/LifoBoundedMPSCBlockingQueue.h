#ifndef FAITHFUL_SRC_EXECUTORS_QUEUES_LIFOUNBOUNDEDSPSCBLOCKINGQUEUE_H_
#define FAITHFUL_SRC_EXECUTORS_QUEUES_LIFOUNBOUNDEDSPSCBLOCKINGQUEUE_H_

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
template <typename Task>
class LifoBoundedMPSCBlockingQueue : public IQueueBase<Task> {
 public:
  using Base = IQueueBase<Task>;
  using Task = Base::Task;

  void Pop() override;
  void Push(Task&&) override;
 private:
  // mutex & condvar
};


} // namespace queue
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_QUEUES_LIFOUNBOUNDEDSPSCBLOCKINGQUEUE_H_
