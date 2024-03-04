#ifndef FAITHFUL_SRC_COMMON_GLOBALSTATEINFO_H_
#define FAITHFUL_SRC_COMMON_GLOBALSTATEINFO_H_

namespace faithful {
namespace details {

class ExecutionEnvironment;

class CollisionManager;
class DrawManager;
class InputManager;
class LoadingManager;
class UpdateManager;

class AudioThreadPool;

struct GlobalStateInfo {
  /// to call Join();
  ExecutionEnvironment* execution_environment;

  /// for task_queue_-s in which each other manager can put tasks
  CollisionManager* collision_manager;
  DrawManager* draw_manager;
  InputManager* input_manager;
  LoadingManager* loading_manager;
  UpdateManager* update_manager;

  /// task_queue_ for sounds; music switching
  /// btw this is the only thread pool (there's 3) which uses it's own queue
  AudioThreadPool* audio_thread_pool;

  inline void StateMenu();
  inline void StateGame();
  inline void StatePaused();

  inline void StateCollisionPaused();
  inline void StateDrawPaused();
  inline void StateInputPaused();
  inline void StateUpdatePaused();

  inline void StateMenuLoadScreen();
  inline void StateMenuStartScreen();
  inline void StateMenuConfigGeneral();
  inline void StateMenuConfigLocalization();
  inline void StateMenuConfigIO();
  inline void StateMenuConfigSound();
  inline void StateMenuConfigGraphic();
  inline void StateMenuConfigKeys();

  inline void StateGameLoadScreen();
  inline void StateGameDefault();
  inline void StateGameInventory1();
  inline void StateGameInventory2();
  inline void StateGameInventory3();
  inline void StateGameInventory4();
  inline void StateGameGamePause();
  inline void StateGameBattle();
};

} // namespace faithful
} // namespace details

#endif  // FAITHFUL_SRC_COMMON_GLOBALSTATEINFO_H_
