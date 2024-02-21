#include "DisplayInteractionThreadPool.h"

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

namespace faithful {
namespace details {

void GlfwErrorCallback(int error __attribute__((unused)),
                       const char *description) {
  fprintf(stderr, "Error: %s\n", description);
}

void GlfwFramebufferSizeCallback(GLFWwindow* window __attribute__((unused)),
                                 int width, int height) {
  glViewport(0, 0, width, height);
}

/// such weird constructor because Cursor and Camera classes have
/// explicitly deleted ctor (there's no sense to use it) and
/// should be initialized with class faithful::Window
DisplayInteractionThreadPool::DisplayInteractionThreadPool(DrawManager* music_manager) :
      window_(), // TODO: deferred window initialization (because of GLFW init)
      camera_game_(&window_),
      cursor_arrow_(&window_, faithful::embedded::cursor_main_menu_data,
                    faithful::embedded::cursor_main_menu_width,
                    faithful::embedded::cursor_main_menu_height),
      cursor_target_(&window_, faithful::embedded::cursor_main_game_data,
                     faithful::embedded::cursor_main_game_width,
                     faithful::embedded::cursor_main_game_height),
      current_cursor_(&cursor_arrow_) {}

void DisplayInteractionThreadPool::Join() {
  // TODO: cur_main_scene_ == cur_game_scene_ == state_ = kTerminated
}

void DisplayInteractionThreadPool::processInput(GLFWwindow *window) {
  if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
    glfwSetWindowShouldClose(window, true);
}

void DisplayInteractionThreadPool::InitContext() {
  if (opengl_initialized_) {
    return;
  }

  if (!glfwInit()) {
    std::cerr << "glfw init error" << std::endl;
    std::terminate();
  }

  glfwSetErrorCallback(GlfwErrorCallback); // TODO: depends on FAITHFUL_DEBUG
  // TODO; the problem is - its already initialized and without GLFW init

  // TODO: init window

  if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
    std::cerr << "Failed to initialize GLAD" << std::endl;
    std::terminate();
  }
  glfwSetFramebufferSizeCallback(window_.Glfw(), GlfwFramebufferSizeCallback);

  glClearColor(0.2, 0.2, 0.2, 1.0);
  std::cout << "OpenGL context initialized" << std::endl;

  opengl_initialized_ = true;
}


void DisplayInteractionThreadPool::Run() {
  InitContext();
  while (faithful::global::game_global_state !=
         faithful::global::GameGlobalState::kTerminate) {
    if (faithful::global::game_global_state ==
        faithful::global::GameGlobalState::kMainMenu) {
      ConfigMainMenu();
      RunMainMenu(); // spinning there
    }
    if (faithful::global::game_global_state ==
        faithful::global::GameGlobalState::kMainGame) {
      ConfigMainGame();
      RunMainGame(); // spinning there
    }
  }
}

void DisplayInteractionThreadPool::RunMainMenu() {
  while (faithful::global::game_global_state ==
         faithful::global::GameGlobalState::kMainMenu) {
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

void DisplayInteractionThreadPool::RunMainGame() {
  while (faithful::global::game_global_state ==
         faithful::global::GameGlobalState::kMainGame) {
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

} // namespace details
} // namespace faithful
