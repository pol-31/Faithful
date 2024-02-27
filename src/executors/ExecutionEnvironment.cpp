#include "ExecutionEnvironment.h"

namespace faithful {
namespace details {

// TODO: player/*, environment/*

// TODO: error handling?

ExecutionEnvironment::ExecutionEnvironment()
    : model_pool_(), music_pool_(), shader_pool_(),
      sound_pool_(), texture_pool_(),
      collision_manager_(&model_pool_),
      draw_manager_(&model_pool_),
      input_manager_(),
      loading_manager_(&model_pool_, &music_pool_, &shader_pool_,
                       &sound_pool_, &texture_pool_),
      update_manager_(&model_pool_),
      audio_thread_pool_(&music_pool_, &sound_pool_),
      display_interaction_thread_pool_(&draw_manager_, &input_manager_),
      game_logic_thread_pool_(&collision_manager_,
                              &loading_manager_, &update_manager_) {
  /// glfw has already been initialized at display_interaction_thread_pool_()
  Init();
}

ExecutionEnvironment::~ExecutionEnvironment() {
  if (state_ == State::kRunning) {
    Join();
  }
  DeInit();
};

void ExecutionEnvironment::Init() {
  global_data_.execution_environment = this;

  global_data_.collision_manager = &collision_manager_;
  global_data_.draw_manager = &draw_manager_;
  global_data_.input_manager = &input_manager_;
  global_data_.loading_manager = &loading_manager_;
  global_data_.update_manager = &update_manager_;
  global_data_.audio_thread_pool = &audio_thread_pool_;

  texture_pool_.SetOpenGlContext(&display_interaction_thread_pool_);

  collision_manager_.SetGlfwWindowUserPointer(&global_data_);
  draw_manager_.SetGlfwWindowUserPointer(&global_data_);
  input_manager_.SetGlfwWindowUserPointer(&global_data_);
  loading_manager_.SetGlfwWindowUserPointer(&global_data_);
  update_manager_.SetGlfwWindowUserPointer(&global_data_);
  /// audio_thread_pool_ don't know about global_data, but there's no need

  display_interaction_thread_pool_.SetGlfwWindowUserPointer(
      reinterpret_cast<void*>(&global_data_));
}

void ExecutionEnvironment::DeInit() {
  display_interaction_thread_pool_.UnSetGlfwWindowUserPointer();
}

void ExecutionEnvironment::Run() {
  state_ = State::kRunning;
  audio_thread_pool_.Run();
  game_logic_thread_pool_.Run();
  display_interaction_thread_pool_.Run(); // called last because it's blocking
}

void ExecutionEnvironment::Join() {
  state_ = State::kJoined;
  /// order doesn't matters, because we only Join(), not calling dtors
  audio_thread_pool_.Join();
  display_interaction_thread_pool_.Join();
  game_logic_thread_pool_.Join();
}

} // namespace faithful
} // namespace details
