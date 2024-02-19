#ifndef FAITHFUL_SCENE_H
#define FAITHFUL_SCENE_H

#include "io/Camera.h"
#include "io/Cursor.h"

namespace faithful {

/// the purpose of this enum class is to check collision only
/// in things what we currently located
// TODO: not sure that's all
enum class SceneType {
  kMainMenu, // including configs; just check different sets of collisions
  kMainGame, // by now only one, but can add mini-games
  kTerminate // useful; should be set if window should be closed
};

/// Scene are in charge of:
/// - which one RenderLoop we're using
/// - NOT for collision. If we want to check collision (to say)
///   on kInventory, we directly ask for it
/// - which and how ThreadPools are working
/// - Camera/Cursor
///TODO: each scene has drawable/collisionable/updatable list?


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
  Scene(Camera camera, details::io::Cursor cursor);

  virtual void ProcessDrawing();
  virtual void ProcessInput();

  void MakeActive();

 protected:
  Camera camera_;
  details::io::Cursor cursor_;
};

class MainMenuScene : public Scene {
 public:
  enum class State {
    kLoadStartScreen, // TODO: for each state its own collision kd-tree
    //                      but it's pregenerated, so can just load from memory
    kLoadPlayScreen,
    kConfigGeneral,
    kConfigLocalization,
    kConfigIO,
    kConfigSound,
    kConfigGraphic,
    kKeys
  };
 private:
};

// SO we have two: STATIC collision list and DYNAMIC collision list

class MainGameScene : public Scene {
 public:
  enum class State {
    kLoadScreen,
    kPlayDefault,
    kInventory1,
    kInventory2,
    kInventory3,
    kInventory4,
    kPause
  };
 private:
};

}  // namespace faithful

#endif  // FAITHFUL_SCENE_H
