#ifndef FAITHFUL_SRC_EXECUTORS_COLLISIONTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_COLLISIONTHREADPOOL_H_

#include "Executor.h"

#include "../collision/CollisionTarget.h"

namespace faithful {

class Collision;

namespace details {

class CollisionManager;

// StaticCollisionCheck -> assume we click1, click2, click3 ->
// -> queue_with_size_1 while processing click1, can't process anything else ->
// -> so if processing time of click1 is longer than time of pressing click2, ->
// -> then click2 skipped and we process only click3

// TODO: thread_num
class CollisionThreadPool : public StaticExecutor<1> {
 public:
  using Base = StaticExecutor<1>;

  CollisionThreadPool() = delete;

  CollisionThreadPool(CollisionManager* collision_manager);

  ~CollisionThreadPool();

  // TODO: add callback
  void CheckDynamic(const Collision* shape, CollisionTarget target);
  void CheckStatic(const Collision* shape, CollisionTarget target);

  void Run() override;
  void Join() override;

 private:
  void InitContext(); // TODO: allocate lhbvh, kdtree
  void DeinitContext(); // TODO: deallocate lhbvh, kdtree

  void RunMainGame();
  void RunMainMenu();

  faithful::LHBVH* dynamic_data_ = nullptr;
  faithful::KDTree* static_data_ = nullptr;

  faithful::LHBVH_result dynamic_data_result;

  CollisionManager* collision_manager_ = nullptr;

  bool initialized_ = false;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_COLLISIONTHREADPOOL_H_
