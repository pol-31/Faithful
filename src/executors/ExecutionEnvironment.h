#ifndef FAITHFUL_SRC_EXECUTORS_EXECUTIONENVIRONMENT_H_
#define FAITHFUL_SRC_EXECUTORS_EXECUTIONENVIRONMENT_H_

#include "IExecutor.h"
#include "../common/GlfwWindowUserPointer.h"

#include "AudioThreadPool.h"
#include "GameLogicThreadPool.h"
#include "DisplayInteractionThreadPool.h"

#include "../common/CollisionManager.h"
#include "../common/DrawManager.h"
#include "../common/InputManager.h"
#include "../common/LoadingManager.h"
#include "../common/UpdateManager.h"

#include "../loader/ModelPool.h"
#include "../loader/MusicPool.h"
#include "../loader/ShaderPool.h"
#include "../loader/SoundPool.h"
#include "../loader/Texture1DPool.h"
#include "../loader/Texture2DPool.h"

namespace faithful {
namespace details {

class ExecutionEnvironment : public IExecutor {
 public:
  ExecutionEnvironment();
  ~ExecutionEnvironment();

  void Run() override;
  void Join() override;

 private:
  void Init();
  void DeInit();

  assets::ModelPool model_pool_;
  assets::MusicPool music_pool_;
  assets::ShaderPool shader_pool_;
  assets::SoundPool sound_pool_;
  assets::Texture1DPool texture1d_pool_;
  assets::Texture2DPool texture2d_pool_;

  CollisionManager collision_manager_;
  DrawManager draw_manager_;
  InputManager input_manager_;
  LoadingManager loading_manager_;
  UpdateManager update_manager_;

  AudioThreadPool audio_thread_pool_;
  DisplayInteractionThreadPool display_interaction_thread_pool_;
  GameLogicThreadPool game_logic_thread_pool_;

  GlfwWindowUserPointer global_data_;
};

} // namespace faithful
} // namespace details

#endif  // FAITHFUL_SRC_EXECUTORS_EXECUTIONENVIRONMENT_H_
