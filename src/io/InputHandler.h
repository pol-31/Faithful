#ifndef FAITHFUL_INPUTHANDLER_H
#define FAITHFUL_INPUTHANDLER_H

#include <forward_list>

#define GLFW_INCLUDE_NONE // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "../../utils/Function.h"
#include "../../utils/Logger.h"
#include "KeyboardKeys.h"

#include "Window.h"

namespace faithful {

class Camera;
enum class SceneMode;

void CallbackMouseButton(GLFWwindow* window [[maybe_unused]],
                         int button, int action, int mods);

/*
void CallbackCursorEnter(GLFWwindow* window, int entered) {
  if (entered) {
    glfwSetCursorPos(window, lastX, lastY);
  }
}

void CallbackCursorPosition(GLFWwindow* window, double xpos, double ypos) {
  std::cout << "cursor moved" << std::endl;
  camera.Look(xpos - lastX, lastY - ypos);
  lastX = xpos;
  lastY = ypos;
}

void CallbackScroll(GLFWwindow* window, double xoffset, double yoffset) {
  camera.Zoom(yoffset);
}


//-________________________________________________________
glfwSetMouseButtonCallback(window, CallbackMouseButton);
glfwSetCursorEnterCallback(window, CallbackCursorEnter);
//glfwSetCursorPosCallback(window, CallbackCursorPosition);
//glfwSetKeyCallback(window, CallbackKey);
glfwSetScrollCallback(window, CallbackScroll);
//-________________________________________________________
*/

class InputHandler {
 public:
  void Run(faithful::Window*) {}

 protected:
  struct KeyCond {
    Key key;
    KeyAction action;
    int mode;
  };
};

// TODO: make only r-value class (?)
// Camera supports two InputHandler-s at once: keyboard and mouse
class KeyboardInputHandler : public InputHandler {
 public:
  KeyboardInputHandler();
  KeyboardInputHandler(SceneMode mode);

  void Bind(Key key, folly::Function<void(void)>&& function,
            KeyAction action = KeyAction::Press, int mode = 0);

  void Unbind();

  // TODO: __KeyCond::mode__ is missing
  void Run(faithful::Window* window);

  void SwitchSceneRequest(int id);

 private:
  friend void StrategyKeyboardHandler(KeyboardInputHandler* input_handler);
  friend void ShooterKeyboardHandler(KeyboardInputHandler* input_handler);
  Window* get_window();
  Camera* get_camera();

  std::forward_list<std::pair<KeyCond,
                    folly::Function<void(void)>*>> action_list_;
};


// the purpose if:
//    continiously track mouse cursor
class MouseInputHandler : public InputHandler {
 public:
  MouseInputHandler();
  MouseInputHandler(SceneMode mode);

  void Run(faithful::Window* window);
  void BindCursorMovement(folly::Function<void(void)>&& fn);

  void ConfigureCursor();

 private:
  // TODO: replace "friend-approach" for the sake of _scalebility_
  friend void ShooterMouseHandler(MouseInputHandler* input_handler);
  friend void StrategyMouseHandler(MouseInputHandler* input_handler);
  Camera* get_camera();

  // TODO: default initialization
  double cur_x;
  double cur_y;
  double last_x;
  double last_y;

  std::unique_ptr<folly::Function<void(void)>> move_cursor_;
  glm::vec2 starting_position_;
};

void StrategyKeyboardHandler(KeyboardInputHandler* input_handler);
void StrategyMouseHandler(MouseInputHandler* input_handler);
void ShooterMouseHandler(MouseInputHandler* input_handler);
void ShooterKeyboardHandler(KeyboardInputHandler* input_handler);

} // namespace faithful

#endif // FAITHFUL_INPUTHANDLER_H
