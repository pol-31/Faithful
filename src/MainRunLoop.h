#ifndef FAITHFUL_SRC_ENGINE_H
#define FAITHFUL_SRC_ENGINE_H

#include "Scene.h"

#include "../utils/Function.h"

namespace faithful {

// Aggregate class (for scenes); switches between Scenes loops
// Camera/Cursor handles by current scene itself
class MainRunLoop {
 public:
  struct FrameRate {
    double last_; // TODO: is float enough?
    double delta_;
  };

  void Run() {
    while (!stop_running_()) {
      while (cur_scene_ == SceneType::kMainMenu) {
        //
      }
      while (cur_scene_ == SceneType::kConfigurations) {
        //
      }
      // .. etc
    }
  }

  void UpdateFramerate();

  // TODO: not sure do we need to switch scene from outside
  static void SwitchSceneRequest(int scene_id);

  // TODO: do we need glfwTerminate() there?

// Deprecated (but still may be useful, see .cpp file):
//  void Init();
//  void StartProcessing();


  static void SetScene(Scene* scene);

 private:
  struct ScenesSet {
    // SceneType::k_1
    // ...
    // SceneType::k_n
  };
  SceneType cur_scene_;
  FrameRate framerate_;
  folly::Function<bool()> stop_running_;
};

}  // namespace faithful

#endif  // FAITHFUL_SRC_ENGINE_H
