#ifndef FAITHFUL_SRC_EXECUTORS_GAMELOGICTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_GAMELOGICTHREADPOOL_H_

#include "IExecutor.h"

#include "../common/FrameRate.h"
#include "../../config/Loader.h"

namespace faithful {
namespace details {

class CollisionManager;
class LoadingManager;
class UpdateManager;

class GameLogicThreadPool
    : public IStaticExecutor<faithful::config::total_game_logic_threads> {
 public:
  using Base = IStaticExecutor<faithful::config::total_game_logic_threads>;

  enum class PriorityState {
    MainMenuLoading, // load embedded assets, no update/collisions
    MainMenu, // no update_manager_,
              // may try to predict further "load game"
    GameStressLoading, // main menu -> load game -> +50 assets
    GameNoLoading, // just standing / boss fight
    GameLazyLoading, // moving somewhere (+tryna predict)
    GameIntensiveLoading // fast moving (assume we run too fast OR fast travel)
  };
  // ONLY GameStressLoading can be set externally, other switches internally

  GameLogicThreadPool() = delete;
  GameLogicThreadPool(CollisionManager* collision_manager,
                      LoadingManager* loading_manager,
                      UpdateManager* update_manager);

  ~GameLogicThreadPool();

  void Run() override;
  void Join() override;

 private:
  enum class ThreadOccupancy {
    kCollision,
    kLoading,
    kUpdate,
    kJoined
  };

  CollisionManager* collision_manager_;
  LoadingManager* loading_manager_;
  UpdateManager* update_manager_;

  std::array<ThreadOccupancy, faithful::config::total_game_logic_threads>
      thread_occupancy_;

  /// We don't use our own task_queue_
  // NOT using Base::task_queue_;

  Framerate framerate_; // TODO: integrate
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_GAMELOGICTHREADPOOL_H_
