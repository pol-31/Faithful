#ifndef FAITHFUL_SRC_EXECUTORS_GAMELOGICTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_GAMELOGICTHREADPOOL_H_

#include <thread>

#include <glm/glm.hpp>

#include "IExecutor.h"

#include "../common/FrameRate.h"
#include "../../config/Loader.h"
#include "../environment/Map.h"

namespace faithful {

class BiomeHandler;

namespace details {

class CollisionManager;
class LoadingManager;
class UpdateManager;

class GameLogicThreadPool
    : public IStaticExecutor<faithful::config::kTotalGameLogicThreads> {
 public:
  using Base = IStaticExecutor<faithful::config::kTotalGameLogicThreads>;

  GameLogicThreadPool() = delete;

  GameLogicThreadPool(CollisionManager* collision_manager,
                      LoadingManager* loading_manager,
                      UpdateManager* update_manager,
                      BiomeHandler* biome_handler,
                      const glm::vec3& player_pos);

  ~GameLogicThreadPool();

  void Run() override;
  void Join() override;

 private:
  enum class PriorityState {
    kDefault,
    kIntensiveLoading, // if current map tiles have not loaded assets
    kStressLoading // previous but + there's a lot
  };

  enum class ThreadOccupancy {
    kCollision,
    kLoading,
    kUpdate,
    kJoined
  };

  void SetDefaultMode();
  void SetIntensiveMode();
  void SetStressMode();

  void UpdateGameMap();

  PriorityState priority_state_;

  CollisionManager* collision_manager_;
  LoadingManager* loading_manager_;
  UpdateManager* update_manager_;

  std::array<ThreadOccupancy, faithful::config::kTotalGameLogicThreads>
      thread_occupancy_;

  /// We don't use task_queue_ there
  // NOT using Base::task_queue_;

  using Base::threads_;
  std::thread lead_thread_;
  bool join_lead_thread_{false};

  MapHandler map_handler_;
  BiomeHandler* biome_handler_;

  const glm::vec3& player_pos_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_GAMELOGICTHREADPOOL_H_
