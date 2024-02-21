#ifndef FAITHFUL_SRC_EXECUTORS_EXECUTIONENVIRONMENT_H_
#define FAITHFUL_SRC_EXECUTORS_EXECUTIONENVIRONMENT_H_

#include "Executor.h"
#include "../common/GlfwWindowUserPointer.h"

namespace faithful {
namespace details {

class AudioThreadPool;
class CollisionThreadPool;
class DisplayInteractionThreadPool;
class UpdateThreadPool;

class ExecutionEnvironment : public Executor {
 public:
  ExecutionEnvironment();
  ExecutionEnvironment(
      AudioThreadPool* audio_thread_pool,
      CollisionThreadPool* collision_thread_pool,
      DisplayInteractionThreadPool* display_interaction_thread_pool,
      UpdateThreadPool* update_thread_pool);

  ~ExecutionEnvironment();

  void Run() override;
  void Join() override;

 private:
  void Init();
  void DeInit();

  AudioThreadPool* audio_thread_pool_;
  CollisionThreadPool* collision_thread_pool_;
  DisplayInteractionThreadPool* display_interaction_thread_pool_;
  UpdateThreadPool* update_thread_pool_;

  GlfwWindowUserPointer global_data;

  bool thread_pools_allocated_;
};

} // namespace faithful
} // namespace details

#endif  // FAITHFUL_SRC_EXECUTORS_EXECUTIONENVIRONMENT_H_
