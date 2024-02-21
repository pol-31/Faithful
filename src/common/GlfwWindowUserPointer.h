#ifndef FAITHFUL_SRC_COMMON_GLFWWINDOWUSERPOINTER_H_
#define FAITHFUL_SRC_COMMON_GLFWWINDOWUSERPOINTER_H_

#include "Scenes.h"
#include "FrameRate.h"

#include <glm/glm.hpp>

namespace faithful {
namespace details {

class ExecutionEnvironment;
class AudioThreadPool;
class CollisionThreadPool;
class DisplayInteractionThreadPool;
class UpdateThreadPool;

struct GlfwWindowUserPointer {
  ExecutionEnvironment* execution_environment;

  AudioThreadPool* audio_thread_pool;
  CollisionThreadPool* collision_thread_pool;
  DisplayInteractionThreadPool* display_interaction_thread_pool;
  UpdateThreadPool* update_thread_pool;

  FaithfulState state;

  const glm::vec2& screen_resolution;
  const bool& need_to_update_monitor_info;
  const Framerate& framerate;
};

} // namespace faithful
} // namespace details

#endif  // FAITHFUL_SRC_COMMON_GLFWWINDOWUSERPOINTER_H_
