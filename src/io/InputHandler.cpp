#include "InputHandler.h"

#include "../../utils/Logger.h"

#include "../Engine.h"

#define GLFW_INCLUDE_NONE  // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "Camera.h"

namespace faithful {

// each frame (60 fps) mouse sends its position to map of
//   all hoverable (buttons, items) and comparing.
// It's looks like collision detection in 2D
/// "Geometric shapes" in our games used solely for collision detection,
/// so it have sense to give them appropriate names
/// alongside with default shapes like AABB, OBB, k-DOP, etc we
/// have hierarchies (BVH, HBVH, kd-tree, etc...) and also
/// optimized hierarchies (like kd-tree but with low partitioning for
/// for example set of buttons, i.e.
/// ________________________________
/// |                              |
/// |------------------------------- #1
/// |                              |
/// |------------------------------- #2
/// |                              |
/// |______________________________|
/// There we check min max of encompassing and then #1, #2, ...
///
/// For real-time collision detection we firstly check 2D and then 3d (if 2d
/// collides)
///
/// collision with point/ray

class Shape2D {};

class MouseSensitiveArea {
 public:
  // TODO: resolution is fixed, so maybe some fixed-size int is enough
  struct Point {
    int x;
    int y;
  };
  // CheckCollision()
 private:
  Shape2D area_;
};

/**
 * Scene 1: заставка, відос, лого компанії і спонсорів
 *     possible input: key_Escape to skip (skip only video)
 *     logo, loading screen - only if it hasn't been loaded yet
 *        (tryna load simultaneously with video, logo, заставка)
 * Scene 2: main screen
 *     possible input: Up, Down, W, S, Enter, Escape (calls PW=pseudo-window:
 * exit y/n) buttons: new game, load game, settings, help(PW), about,
 * resources/data, quit(PW)
 * */

/*TODO:
 * 1) delete everything
 * 2) check what GLFW suggest to us
 * 3) create _completed_set_ of input_modes
 *So we have:
 * menu: new game, load game, settings, help, about, resources/data, quit
 *   // menu changes depends on player progress as well as content of
 *resources/data There are 6 parts of game (Gothic-like), where territory may be
 *      generally the same but with some differences:
 *      Part I:
 *        like Spring-month, friendly atmosphere, all around is "green-live",
 *relatively small area Part II: all seems to be the same + new territory, where
 *player can find new friends/enemies, collaborations Part III: start of Big
 *Game showed all HUGE area; main part of game something bad happened, so now
 *everything is not the same but being degradated: not-green, liquids starting
 *dry, some locations become dry, some in fire, some "frostbited", some
 *forbidden, etc. (as in location that we consider for previous game) Need to
 *find N artifacts (there is N biomes) Part IV: bigger nature degradation, some
 *extra locations(hidden), more location-specific weather, new creatures "not
 *from our world" Need to kill/пощадить N bosses from each locations, banishing
 *evil "from our lands" Part V: before final extra bosses, extra locations -
 *dangeons (randomly generated) Part VI: end of the World, full collapse Part
 *VII: realizing, who we are and that all was our imagination... (or not?)
 *
 *
 *
 *
 *
 * */

void CallbackMouseButton(GLFWwindow* window [[maybe_unused]], int button,
                         int action, int mods) {
  double xpos, ypos;
  glfwGetCursorPos(faithful::SimurghManager::get_window()->Glfw(), &xpos,
                   &ypos);
  if (button == GLFW_MOUSE_BUTTON_RIGHT && action == GLFW_PRESS) {
    // Log(LogType::kInfo) << "right mouse button clicked " << xpos << " " <<
    // ypos;
  } else if (button == GLFW_MOUSE_BUTTON_LEFT && action == GLFW_PRESS) {
    // Log(LogType::kInfo) << "lest mouse button clicked " << xpos << " " <<
    // ypos;
  }
}

void MouseInputHandler::ConfigureCursor() {
  // TODO: also add callbacks here
  //          _______________________       default_position
  // TODO: we should set cursor_position here
}

void KeyboardInputHandler::SwitchSceneRequest(int id) {
  SimurghManager::SwitchSceneRequest(id);
}

KeyboardInputHandler::KeyboardInputHandler() {
}
KeyboardInputHandler::KeyboardInputHandler(SceneMode mode) {
  switch (mode) {
    case SceneMode::Shooter:
      ShooterKeyboardHandler(this);
      break;
    case SceneMode::Rpg:
      break;
    case SceneMode::Strategy:
      StrategyKeyboardHandler(this);
      break;
    case SceneMode::Platformer:
      break;
  }
}

MouseInputHandler::MouseInputHandler() {
}
MouseInputHandler::MouseInputHandler(SceneMode mode) {
  switch (mode) {
    case SceneMode::Shooter:
      ShooterMouseHandler(this);
      break;
    case SceneMode::Rpg:
      break;
    case SceneMode::Strategy:
      StrategyMouseHandler(this);
      break;
    case SceneMode::Platformer:
      break;
  }
}

// TODO: __KeyCond::mode__ is missing
void KeyboardInputHandler::Run(Window* window) {
  // TODO: deltatime does nothing
  if (action_list_.empty())
    return;
  for (auto i : action_list_) {
    if (glfwGetKey(window->Glfw(), static_cast<int>(i.first.key)) ==
        static_cast<int>(i.first.action)) {
      (*i.second)();
    }
  }
}

void MouseInputHandler::Run(faithful::Window* window) {
  glfwGetCursorPos(window->Glfw(), &cur_x, &cur_y);
  if (move_cursor_ == nullptr)
    return;
  (*move_cursor_)();
  // TODO: .. other key processing
}

void MouseInputHandler::BindCursorMovement(folly::Function<void(void)>&& fn) {
  auto upf = std::unique_ptr<folly::Function<void(void)>>();
  upf = std::make_unique<folly::Function<void(void)>>(std::move(fn));
  move_cursor_ = std::move(upf);
}

void KeyboardInputHandler::Bind(Key key, folly::Function<void(void)>&& function,
                                KeyAction action, int mode) {
  // Check: if MouseButtons/scroll -> add to Callback
  auto pair =
      std::make_pair(KeyCond{key, action, mode},
                     new folly::Function<void(void)>(std::move(function)));
  action_list_.emplace_front(std::move(pair));
}

void KeyboardInputHandler::Unbind() {
}

Camera* MouseInputHandler::get_camera() {
  return SimurghManager::get_camera();
}

// mark as friend, so we could use private: cur_x, cur_y, last_x, last_y;
void ShooterMouseHandler(MouseInputHandler* input_handler) {
  input_handler->BindCursorMovement([=]() {
    (input_handler->get_camera())
        ->Look(input_handler->cur_x - input_handler->last_x,
               input_handler->last_y - input_handler->cur_y);
    input_handler->last_x = input_handler->cur_x;
    input_handler->last_y = input_handler->cur_y;
  });
}

void StrategyMouseHandler(MouseInputHandler* input_handler) {
}

Window* KeyboardInputHandler::get_window() {
  return SimurghManager::get_window();
}
Camera* KeyboardInputHandler::get_camera() {
  return SimurghManager::get_camera();
}

void ShooterKeyboardHandler(KeyboardInputHandler* input_handler) {
  input_handler->Bind(Key::Escape, [input_handler]() {
    glfwSetWindowShouldClose(input_handler->get_window()->Glfw(), true);
  });
  input_handler->Bind(Key::W, [input_handler]() {
    (input_handler->get_camera())->Move(Camera::MoveDirection::Forward);
  });
  input_handler->Bind(Key::S, [input_handler]() {
    (input_handler->get_camera())->Move(Camera::MoveDirection::Back);
  });
  input_handler->Bind(Key::A, [input_handler]() {
    (input_handler->get_camera())->Move(Camera::MoveDirection::Left);
  });
  input_handler->Bind(Key::D, [input_handler]() {
    (input_handler->get_camera())->Move(Camera::MoveDirection::Right);
  });
}

void StrategyKeyboardHandler(KeyboardInputHandler* input_handler) {
  input_handler->Bind(Key::Escape, [input_handler]() {
    glfwSetWindowShouldClose(input_handler->get_window()->Glfw(), true);
  });
  input_handler->Bind(Key::W, [input_handler]() {
    (input_handler->get_camera())->Move(Camera::MoveDirection::Up);
  });
  input_handler->Bind(Key::S, [input_handler]() {
    (input_handler->get_camera())->Move(Camera::MoveDirection::Down);
  });
  input_handler->Bind(Key::A, [input_handler]() {
    (input_handler->get_camera())->Move(Camera::MoveDirection::Left);
  });
  input_handler->Bind(Key::D, [input_handler]() {
    (input_handler->get_camera())->Move(Camera::MoveDirection::Right);
  });
}

}  // namespace faithful
