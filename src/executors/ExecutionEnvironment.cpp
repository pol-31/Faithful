#include "ExecutionEnvironment.h"

#include "AudioThreadPool.h"
#include "CollisionThreadPool.h"
#include "DisplayInteractionThreadPool.h"
#include "UpdateThreadPool.h"

#include "../loader/SoundPool.h"
#include "../loader/MusicPool.h"

#include "../common/CollisionManager.h"
#include "../common/DrawManager.h"
#include "../common/UpdateManager.h"

#include <assert.h>

namespace faithful {
namespace details {


//TODO 2: (ExecutionEnvironment.h):
// framerate <-- Update()_read, Draw()_write
// screen resolution <-- Draw()_read, GlobalCallback_write
// GlfwWindowUserPointer:
// - this; (to call Join()) --------------- execution_env
// - all_ThreadPools --------------- execution_env
// - all_SceneTypes; --------------- execution_env
// - screen_res; --------------- display_interaction_thread_pool (window)
// - need_to_update_monitor; --------------- display_interaction_thread_pool (window)
// - framerate(?). --------------- display_interaction_thread_pool

// TODO 3: always need to use faithful::Window, NOT GLFWwindow* !!!

// TODO 4: AudioThreadPool

ExecutionEnvironment::ExecutionEnvironment() {
  thread_pools_allocated_ = true;

  // TODO: you know what to do with these managers...

  // TODO: we don't need HEAP memory there

  auto sound_pool = new assets::SoundPool; // delete
  auto music_pool = new assets::MusicPool; // delete

  auto collision_manager = new CollisionManager; // delete
  auto draw_manager = new DrawManager; // delete
  auto update_manager = new UpdateManager; // delete

  audio_thread_pool_ = new AudioThreadPool(music_pool, sound_pool);
  collision_thread_pool_ = new CollisionThreadPool(collision_manager);
  display_interaction_thread_pool_ = new DisplayInteractionThreadPool(draw_manager);
  update_thread_pool_ = new UpdateThreadPool(update_manager);

  global_data.execution_environment = this;
  global_data.audio_thread_pool = audio_thread_pool_;
  global_data.collision_thread_pool = collision_thread_pool_;
  global_data.display_interaction_thread_pool = display_interaction_thread_pool_;
  global_data.update_thread_pool = update_thread_pool_;
  //TODO: global_data.framerate, screen_resolution,
  // need_to_update_monitor_info - from display_interaction_thread_pool::window_
}

ExecutionEnvironment::ExecutionEnvironment(
    AudioThreadPool* audio_thread_pool,
    CollisionThreadPool* collision_thread_pool,
    DisplayInteractionThreadPool* display_interaction_thread_pool,
    UpdateThreadPool* update_thread_pool) {
  thread_pools_allocated_ = false;

  assert(audio_thread_pool != nullptr);
  assert(collision_thread_pool != nullptr);
  assert(display_interaction_thread_pool != nullptr);
  assert(update_thread_pool != nullptr);

  audio_thread_pool_ = audio_thread_pool;
  collision_thread_pool_ = collision_thread_pool;
  display_interaction_thread_pool_ = display_interaction_thread_pool;
  update_thread_pool_ = update_thread_pool;
}

ExecutionEnvironment::~ExecutionEnvironment() {
  if (thread_pools_allocated_) {
    delete audio_thread_pool_;
    delete collision_thread_pool_;
    delete display_interaction_thread_pool_;
    delete update_thread_pool_;
  }
};

void ExecutionEnvironment::Init() {
  // create managers(sound, music, draw/update/collision) if needed
}

void ExecutionEnvironment::DeInit() {
  // delete managers(sound, music, draw/update/collision) if needed
}

void ExecutionEnvironment::Run() {
  audio_thread_pool_->Run();
  collision_thread_pool_->Run();
  update_thread_pool_->Run();
  // called last because it's blocking
  display_interaction_thread_pool_->Run();
}

void ExecutionEnvironment::Join() {
  audio_thread_pool_->Join();
  collision_thread_pool_->Join();
  update_thread_pool_->Join();
  // called last because it blocks main() which which should be stopped last
  display_interaction_thread_pool_->Join();
}

} // namespace faithful
} // namespace details
