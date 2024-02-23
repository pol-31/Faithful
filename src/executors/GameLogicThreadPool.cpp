#include "GameLogicThreadPool.h"

#include "../common/CollisionManager.h"
#include "../common/LoadingManager.h"
#include "../common/UpdateManager.h"

namespace faithful {
namespace details {

GameLogicThreadPool::GameLogicThreadPool(
    CollisionManager* collision_manager,
    LoadingManager* loading_manager,
    UpdateManager* update_manager)
    : collision_manager_(collision_manager),
      loading_manager_(loading_manager),
      update_manager_(update_manager) {}

GameLogicThreadPool::~GameLogicThreadPool() {
  if (state_ != IExecutor::State::kJoined) {
    Join();
  }
}

void GameLogicThreadPool::Run() {
  state_ = IExecutor::State::kRunning;
  for (std::size_t i = 0; i < threads_.size(); ++i) {
    threads_[i] = std::thread{[&, i]{
      while(true) {
        // TODO: how to switch threads? Call Pause for all?
        if (thread_occupancy_[i] == ThreadOccupancy::kCollision) {
          collision_manager_->Run();
        } else if (thread_occupancy_[i] == ThreadOccupancy::kLoading) {
          loading_manager_->Run();
        } else if (thread_occupancy_[i] == ThreadOccupancy::kUpdate) {
          update_manager_->Run();
        } else { // thread_occupancy_[i] == ThreadOccupancy::kJoined
          break;
        }
      }
    }};
  }
}

void GameLogicThreadPool::Join() {
  state_ = State::kJoined;
  for (auto& thread_occupancy : thread_occupancy_) {
    thread_occupancy = ThreadOccupancy::kJoined;
  }
  for (auto& thread : threads_) {
    thread.join();
  }
}

} // namespace details
} // namespace faithful
