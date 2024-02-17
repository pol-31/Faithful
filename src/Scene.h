#ifndef FAITHFUL_SCENE_H
#define FAITHFUL_SCENE_H

#include <set>

#include "common/Object.h"
#include "entities/EntityTraits.h"

namespace faithful {

/// the purpose of this enum class is to check collision only
/// in things what we currently located
// TODO: not sure that's all
enum class SceneType {
  kMainMenu,
  kInventory,
  kMainGameHud,
  kMainGameItem, // differs from hud, that it's dynamic
  kConfigurations
};

/// Scene are in charge of:
/// - which one RenderLoop we're using
/// - NOT for collision. If we want to check collision (to say)
///   on kInventory, we directly ask for it
/// - which and how ThreadPools are working
/// - Camera/Cursor
///TODO: each scene has drawable/collisionable/updatable list?

class Camera;
class Cursor;

/** How we're dealing with different scenes:
 *
 * Scene scenes[5];
 * for (int i = 0; i < 5; ++i) {
 *   scenes[i].Config(); <--- all of them has different type (menu/game/)
 * }
 * while(true / window_close) {
 *   if (scene == SceneType::kMainMenu) {
 *     scene[kMainMenu].RunLoop();
 *   } else if (...) {...}
 * }
 *
 * */

// TODO: create main Scene class which will handle all scenes
//    and its function Run(), described above. As an argument
//    we need lambda stop_working, like [](){ return window.IsClosed(); }



class Scene {
 public:
  // see Scene(Dummy, int id) below
  struct Dummy {};

  Scene();
  Scene(const Scene& other) = delete;
  Scene(Scene&& other) = delete;
  Scene& operator=(const Scene& other) = delete;
  Scene& operator=(Scene&& other) = delete;

  ~Scene();


  int Id() const {
    return id_;
  }
  virtual void ProcessDrawing() {
  }
  void ProcessInput(Window* window);
  /// animations
  /// rigid body
  /// physics
  // TODO: void ProcessUpdateObj();
  // TODO: create class UpdateManager;
  // TODO: create class Updatable - user has to just inherit from it
  //              and override Update() method.
  //  ____We also want class UpdateableUbiqutous and then just put lambdas into
  //  it

  // TODO: std::forward_list<const SingleObjectImpl__not_complete*>*
  // update_list_ = nullptr;

  /// __subscribers__-like system
  // void ProcessCollisions();
  // void ProcessAudio();

  Camera* get_camera() const {
    return camera_;
  }
  Cursor* get_cursor() const {
    return cursor_;
  }
  // RenderSequence* get_draw_list() const { return draw_list_; }

  void set_camera(Camera* camera) {
    camera_ = camera;
  }
  void set_cursor(Cursor* cursor) {
    cursor_ = cursor;
  }

 protected:
  Camera* camera_ = nullptr;
  Cursor* cursor_ = nullptr;
  // Managers?
};


}  // namespace faithful

#endif  // FAITHFUL_SCENE_H
