#include "RenderThreadPool.h"

#include "../../utils/Logger.h"

#include "queues/LifoBoundedMPSCBlockingQueue.h"

namespace faithful {
namespace details {

// TODO: .h
static void error_callback(int error, const char *description) {
  (void)error;
  fprintf(stderr, "Error: %s\n", description);
}


RenderThreadPool::RenderThreadPool() {
  task_queue_ = new queue::LifoBoundedMPSCBlockingQueue<Task>;
}
RenderThreadPool::~RenderThreadPool() {
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
  while (cur_scene_ != SceneType::kTerminate) {// TODO: set WindowShouldBeClosed callback
    RunMainMenuScene();
    RunMainGameScene();
  }
}

void RenderThreadPool::RunMainMenuScene() {
  if (cur_scene_ == SceneType::kMainMenu) {
    // TODO: config MainMenu scene
  }
  while (cur_scene_ == SceneType::kMainMenu) {
    UpdateFramerate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window_.Glfw());
    glfwPollEvents();
  }
}
void RenderThreadPool::RunMainGameScene() {
  if (cur_scene_ == SceneType::kMainMenu) {
    // TODO: config MainGame scene
  }
  while (cur_scene_ == SceneType::kMainGame) {
    UpdateFramerate();
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    glfwSwapBuffers(window_.Glfw());
    glfwPollEvents();
  }
}

void RenderThreadPool::UpdateFramerate() {
  double current_frame = glfwGetTime();
  framerate_.delta_ = current_frame - framerate_.last_;
  framerate_.last_ = current_frame;
}

} // namespace details
} // namespace faithful
