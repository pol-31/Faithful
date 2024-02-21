#ifndef FAITHFUL_SRC_EXECUTORS_UPDATETHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_UPDATETHREADPOOL_H_

#include "Executor.h"

namespace faithful {
namespace details {

class UpdateManager;

// TODO: thread_num
class UpdateThreadPool : public StaticExecutor<1> {
 public:
  using Base = StaticExecutor<1>;

  UpdateThreadPool() = delete;

  UpdateThreadPool(UpdateManager* collision_manager);

  ~UpdateThreadPool();

  // TODO: add callback
  void Schedule();

  void Run() override;
  void Join() override;

 private:
  void InitContext(); // TODO: allocate storage
  void DeinitContext(); // TODO: deallocate storage

  void RunMainGame();
  void RunMainMenu();

  UpdateManager* collision_manager_ = nullptr;

  bool initialized_ = false;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_UPDATETHREADPOOL_H_
