#include "GameLogicThreadPool.h"

#include "../common/CollisionManager.h"
#include "../common/LoadingManager.h"
#include "../common/UpdateManager.h"
#include "../environment/Biome.h"

namespace faithful {
namespace details {

GameLogicThreadPool::GameLogicThreadPool(
    CollisionManager* collision_manager,
    LoadingManager* loading_manager,
    UpdateManager* update_manager,
    BiomeHandler* biome_handler,
    const glm::vec3& player_pos)
    : collision_manager_(collision_manager),
      loading_manager_(loading_manager),
      update_manager_(update_manager),
      map_handler_(loading_manager),
      biome_handler_(biome_handler),
      player_pos_(player_pos) {
  SetDefaultMode();
}

GameLogicThreadPool::~GameLogicThreadPool() {
  if (state_ != IExecutor::State::kJoined) {
    Join();
  }
}

void GameLogicThreadPool::Run() {
  state_ = IExecutor::State::kRunning;
  lead_thread_ = std::thread{[&]{
    while(true) {
      UpdateGameMap();
      collision_manager_->UpdateTree();
      update_manager_->UpdateAnimations();
      update_manager_->UpdateGameLogic();
      if (join_lead_thread_) {
        break;
      }
    }
  }};

  for (std::size_t i = 1; i < threads_.size(); ++i) {
    threads_[i] = std::thread{[&, i]{
      while(true) {
        while (thread_occupancy_[i] == ThreadOccupancy::kCollision) {
          collision_manager_->ProcessTask();
        }
        while (thread_occupancy_[i] == ThreadOccupancy::kLoading) {
          loading_manager_->ProcessTask();
        }
        while (thread_occupancy_[i] == ThreadOccupancy::kUpdate) {
          update_manager_->ProcessTask();
        }
        if (thread_occupancy_[i] == ThreadOccupancy::kJoined) {
          break;
        }
      }
    }};
  }
}

void GameLogicThreadPool::Join() {
  state_ = State::kJoined;
  join_lead_thread_ = true;
  for (auto& thread_occupancy : thread_occupancy_) {
    thread_occupancy = ThreadOccupancy::kJoined;
  }
  for (auto& thread : threads_) {
    thread.join();
  }
}

//////// IMPORTANT ////////
/// currently there's only 5 threads

void GameLogicThreadPool::SetDefaultMode() {
  if (priority_state_ == PriorityState::kDefault) {
    return;
  }
  priority_state_ = PriorityState::kDefault;
  thread_occupancy_[0] = ThreadOccupancy::kCollision;
  thread_occupancy_[1] = ThreadOccupancy::kCollision;
  thread_occupancy_[2] = ThreadOccupancy::kLoading;
  thread_occupancy_[3] = ThreadOccupancy::kUpdate;
  thread_occupancy_[4] = ThreadOccupancy::kUpdate;
}

void GameLogicThreadPool::SetIntensiveMode() {
  if (priority_state_ == PriorityState::kIntensiveLoading) {
    return;
  }
  priority_state_ = PriorityState::kIntensiveLoading;
  thread_occupancy_[0] = ThreadOccupancy::kCollision;
  thread_occupancy_[1] = ThreadOccupancy::kLoading;
  thread_occupancy_[2] = ThreadOccupancy::kLoading;
  thread_occupancy_[3] = ThreadOccupancy::kLoading;
  thread_occupancy_[4] = ThreadOccupancy::kUpdate;
}

void GameLogicThreadPool::SetStressMode() {
  if (priority_state_ == PriorityState::kStressLoading) {
    return;
  }
  priority_state_ = PriorityState::kStressLoading;
  thread_occupancy_[0] = ThreadOccupancy::kLoading;
  thread_occupancy_[1] = ThreadOccupancy::kLoading;
  thread_occupancy_[2] = ThreadOccupancy::kLoading;
  thread_occupancy_[3] = ThreadOccupancy::kLoading;
  thread_occupancy_[4] = ThreadOccupancy::kLoading;
  loading_manager_->StressLoading(/* +4 threads by default */);
}

void GameLogicThreadPool::UpdateGameMap() {
  auto status = map_handler_.UpdateTiles({player_pos_.x, player_pos_.z});
  if (status == MapHandler::LoadingStatus::kCalm) {
    SetDefaultMode();
  } else if (status == MapHandler::LoadingStatus::kIntensive) {
    SetIntensiveMode();
  } else if (status == MapHandler::LoadingStatus::kStress) {
    SetStressMode();
  }
  biome_handler_->SetBiome(map_handler_.CurBiome());
}

} // namespace details
} // namespace faithful
