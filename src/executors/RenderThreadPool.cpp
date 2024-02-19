#include "RenderThreadPool.h"

#include "../../utils/Logger.h"

#include "../../assets/embedded/CursorMainMenu.h"
#include "../../assets/embedded/CursorMainGame.h"

#include "queues/LifoBoundedMPSCBlockingQueue.h"

namespace faithful {
namespace details {

// TODO: .h
static void error_callback(int error, const char *description) {
  (void)error;
  fprintf(stderr, "Error: %s\n", description);
}


RenderThreadPool::RenderThreadPool() :
      window_(),
      camera_game_(&window_),
      cursor_arrow_(&window_, faithful::embedded::cursor_main_menu_data,
                    faithful::embedded::cursor_main_menu_width,
                    faithful::embedded::cursor_main_menu_height),
      cursor_target_(&window_, faithful::embedded::cursor_main_game_data,
                     faithful::embedded::cursor_main_game_width,
                     faithful::embedded::cursor_main_game_height),
      current_cursor_(&cursor_arrow_) {
  task_queue_ = new queue::LifoBoundedMPSCBlockingQueue<Task>;
}
RenderThreadPool::~RenderThreadPool() {
  Camera camera(&window_);
  delete task_queue_;
  glfwTerminate();
}

void RenderThreadPool::Join() {
  state_ = State::kJoined;
  // TODO: Join() from main thread and Run() from AudioThread intersecting there
  //   need synchronization
  while (!task_queue_->Empty()) {
    (task_queue_->Front())();
  }
  /*for (auto& thread : threads_) {
    thread.join();
  }*/
}


#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

void processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void RenderThreadPool::InitOpenGLContext() {
  if (opengl_initialized_) {
    return;
  }

  if (!glfwInit()) {
    std::cerr << "glfw init error" << std::endl;
    std::terminate();
  }
  glfwSetErrorCallback(error_callback);

  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  if (!window_.Init()) {
    std::cout << "Failed to create GLFW window" << std::endl;
    glfwTerminate();
    std::terminate();
  }

  glfwMakeContextCurrent(window_.Glfw());
  glClearColor(0.2, 0.2, 0.2, 1.0);

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    std::terminate();
  }

  std::cout << "OpenAL context initialized" << std::endl;

  opengl_initialized_ = true;
}
void RenderThreadPool::DeinitOpenGLContext() {
  if (opengl_initialized_) {
    // TODO: blocking call glfwWindowsShouldStop
    glfwTerminate();
    opengl_initialized_ = false;
  }
}

void RenderThreadPool::Run() {
  if (state_ != State::kNotStarted) {
    return;
  }
  while (game_state_ != GameState::kTerminate) {// TODO: set WindowShouldBeClosed callback
    if (game_state_ == GameState::kMainMenu) {
      ConfigMainMenu();
      RunMainMenu(); // spinning there
    }
    if (game_state_ == GameState::kMainGame) {
      ConfigMainGame();
      RunMainGame(); // spinning there
    }
  }
}

void RenderThreadPool::RunMainMenu() {
  while (game_state_ == GameState::kMainMenu) { // TODO: state joint for all?
    switch (cur_menu_scene_) {
      case MainMenuSceneState::kLoadStartScreen:
        ConfigLoadStartScreen();
        while (cur_menu_scene_ == MainMenuSceneState::kLoadStartScreen) {
          DrawLoadStartScreen();
          ProcessInputLoadStartScreen();
        }
        break;
      case MainMenuSceneState::kLoadPlayScreen:
        ConfigLoadPlayScreen();
        while (cur_menu_scene_ == MainMenuSceneState::kLoadStartScreen) {
          DrawLoadPlayScreen();
          ProcessInputLoadPlayScreen();
        }
        break;
      case MainMenuSceneState::kConfigGeneral:
        ConfigConfigGeneral();
        while (cur_menu_scene_ == MainMenuSceneState::kLoadPlayScreen) {
          DrawConfigGeneral();
          ProcessInputConfigGeneral();
        }
        break;
      case MainMenuSceneState::kConfigLocalization:
        ConfigConfigLocalization();
        while (cur_menu_scene_ == MainMenuSceneState::kConfigGeneral) {
          DrawConfigLocalization();
          ProcessInputConfigLocalization();
        }
        break;
      case MainMenuSceneState::kConfigIO:
        ConfigConfigIO();
        while (cur_menu_scene_ == MainMenuSceneState::kConfigLocalization) {
          DrawConfigIO();
          ProcessInputConfigIO();
        }
        break;
      case MainMenuSceneState::kConfigSound:
        ConfigConfigSound();
        while (cur_menu_scene_ == MainMenuSceneState::kConfigIO) {
          DrawConfigSound();
          ProcessInputConfigSound();
        }
        break;
      case MainMenuSceneState::kConfigGraphic:
        ConfigConfigGraphic();
        while (cur_menu_scene_ == MainMenuSceneState::kConfigSound) {
          DrawConfigGraphic();
          ProcessInputConfigGraphic();
        }
        break;
      case MainMenuSceneState::kKeys:
        ConfigKeys();
        while (cur_menu_scene_ == MainMenuSceneState::kConfigGraphic) {
          DrawKeys();
          ProcessInputKeys();
        }
        break;
      default:
        std::cerr << "Incorrect MainMenuSceneState type, switching to default"
                  << std::endl;
        cur_menu_scene_ = MainMenuSceneState::kLoadPlayScreen;
    }
  }
}

void RenderThreadPool::RunMainGame() {
  while (game_state_ == GameState::kMainGame) { // TODO: state joint for all?
    switch (cur_game_scene_) {
      case MainGameSceneState::kGameLoadScreen:
        ConfigGameLoadScreen();
        while (cur_game_scene_ == MainGameSceneState::kGameLoadScreen) {
          DrawGameLoadScreen();
          ProcessInputGameLoadScreen();
        }
        break;
      case MainGameSceneState::kGameDefault:
        ConfigGameDefault();
        while (cur_game_scene_ == MainGameSceneState::kGameDefault) {
          DrawGameDefault();
          ProcessInputGameDefault();
        }
        break;
      case MainGameSceneState::kInventory1:
        ConfigInventory1();
        while (cur_game_scene_ == MainGameSceneState::kInventory1) {
          DrawInventory1();
          ProcessInputInventory1();
        }
        break;
      case MainGameSceneState::kInventory2:
        ConfigInventory2();
        while (cur_game_scene_ == MainGameSceneState::kInventory2) {
          DrawInventory2();
          ProcessInputInventory2();
        }
        break;
      case MainGameSceneState::kInventory3:
        ConfigInventory3();
        while (cur_game_scene_ == MainGameSceneState::kInventory3) {
          DrawInventory3();
          ProcessInputInventory3();
        }
        break;
      case MainGameSceneState::kInventory4:
        ConfigInventory4();
        while (cur_game_scene_ == MainGameSceneState::kInventory4) {
          DrawInventory4();
          ProcessInputInventory4();
        }
        break;
      case MainGameSceneState::kGamePause:
        ConfigGamePause();
        while (cur_game_scene_ == MainGameSceneState::kGamePause) {
          DrawGamePause();
          ProcessInputGamePause();
        }
        break;
      default:
        std::cerr << "Incorrect MainGameSceneState type, switching to default"
                  << std::endl;
        cur_game_scene_ = MainGameSceneState::kGameDefault;
    }
  }
}

void RenderThreadPool::UpdateFramerate() {
  double current_frame = glfwGetTime();
  framerate_.delta_ = current_frame - framerate_.last_;
  framerate_.last_ = current_frame;
}

} // namespace details
} // namespace faithful
