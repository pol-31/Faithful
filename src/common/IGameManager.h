#ifndef FAITHFUL_SRC_COMMON_IGAMEMANAGER_H_
#define FAITHFUL_SRC_COMMON_IGAMEMANAGER_H_

#include "../executors/queues/IQueue.h"

#include "../../utils/Function.h"

namespace faithful {
namespace details {

struct GlfwWindowUserPointer;

class IGameManager {
 public:
  using TaskType = folly::Function<void()>;

  IGameManager() = default;
  virtual ~IGameManager() = default;

  /// not copyable
  IGameManager(const IGameManager&) = delete;
  IGameManager& operator=(const IGameManager&) = delete;

  /// movable
  IGameManager(IGameManager&&) = default;
  IGameManager& operator=(IGameManager&&) = default;

  TaskType&& Take() {
    task_queue_->Pop();
  }

  void Put(TaskType&& task) {
    task_queue_->Push(std::move(task));
  }

  void SetGlfwWindowUserPointer(
      GlfwWindowUserPointer* glfw_window_user_pointer) {
    glfw_window_user_pointer_ = glfw_window_user_pointer;
  }

  /// Update() for 1 iteration, Run() for spinning
  virtual void Update() = 0;
  virtual void Run() = 0;

 protected:
  queue::IQueueBase<TaskType>* task_queue_;
  GlfwWindowUserPointer* glfw_window_user_pointer_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_COMMON_IGAMEMANAGER_H_
