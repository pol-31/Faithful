#ifndef FAITHFUL_ENGINE_H
#define FAITHFUL_ENGINE_H

#include <thread>

#define GLFW_INCLUDE_NONE // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include "../utils/Logger.h"

#include "io/Camera.h"
#include "io/Cursor.h"
#include "io/Window.h"

#include "../utils/Executors/ThreadPools.h"

namespace faithful {

class Scene;

class RenderThreadPool;
class LoadThreadPool;

class KeyboardInputHandler;

void ProcessInput(GLFWwindow* window, Camera *camera, double deltaTime);

class SimurghManager {
 public:

  struct FrameRate {
    double last_;
    double delta_;
  };
  SimurghManager();

   static void set_camera(Camera* camera);
  static void set_window(Window* window) {
    window_ = window;
  }
  static void set_scene(Scene* scene);

  static void SwitchSceneRequest(int scene_id);

  static Window* get_window() {
    return window_;
  }
  static double get_framerate() {
    return framerate_.delta_;
  }
  static Camera* get_camera();
  static Scene* get_scene();
  static ThreadPoolManager* get_thraed_pool_manager() {
    return thread_pool_manager_;
  }

  static void Initialization() {
    initialized_ = true;
  }

 private:
  void Init();
  void StartProcessing();

  static void ProcessOpenGLFunctions();


  //-------------------------
  // object do it for itself:___ process all collisions -- > thread-pool
  /// each object if it want should check collision with all other objects on the scene
  /// useful optimization: segregate all objects in enum{kBig, kMedium, kSmall} and
  /// check only those which located in the same cells(kSmall) / in the nearby 3 cells(kMedium) / everywhere(kBig)
  /// so we DON'T need to process it HERE
  //-------------------------

  static void UpdateFramerate();

  static Window* window_; // default depends on monitor resolution
  static FrameRate framerate_;
  static Scene* scene_;
  static ThreadPoolManager* thread_pool_manager_;
  static Logger* logger_;

  static bool initialized_;
};

RenderThreadPool* CurrentRenderThreadPool();
LoadThreadPool* CurrentLoadThreadPool();
SoundThreadPool* CurrentSoundThreadPool();
ObjectThreadPool* CurrentObjectThreadPool();
Window* CurrentWindow();
glm::ivec2 CurrentResolution();

} // namespace faithful


#endif // FAITHFUL_ENGINE_H
