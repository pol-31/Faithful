#include "MainRunLoop.h"

namespace faithful {

void MainRunLoop::UpdateFramerate() {
  double current_frame = glfwGetTime();
  framerate_.delta_ = current_frame - framerate_.last_;
  framerate_.last_ = current_frame;
}

void MainRunLoop::SwitchSceneRequest(int scene_id) {
  Scene* target_scene = SceneManager::SceneById(scene_id);
  if (target_scene != nullptr)
    set_scene(target_scene);
}

// Deprecated mechanic:

/*
void MainRunLoop::Init() {
  logger_ = CreateLogger();
  thread_pool_manager_ = new ThreadPoolManager;

  thread_pool_manager_->get_render_thread_pool()->Config([=]() {
    logger_->LogIf(faithful::LogType::kFatal, !glfwInit(),
                   "Unable to init GLFW");

    SimurghManager::set_window(new Window());
    logger_->LogIf(faithful::LogType::kFatal, !gladLoaderLoadGL(),
                   "Unable to init GLAD");

    // TODO: init there all static-dur things

    SimurghManager::set_scene(new Scene3D());
    SimurghManager::get_scene()->MakeActive();
  });

  StartProcessing();

  while (!initialized_) {
  }
}

void MainRunLoop::StartProcessing() {
  framerate_.last_ = glfwGetTime();
  UpdateFramerate();

  thread_pool_manager_->get_render_thread_pool()->Config([=]() {
    glClearColor(0.3f, 0.3f, 0.8f, 1.0f);  // TODO: to class_Scene
    glfwSwapInterval(1);                   // Vsync - window property
  });

  thread_pool_manager_->get_render_thread_pool()->AttachRenderLoop([&]() {
    UpdateFramerate();

    faithful::SimurghManager::get_scene()->ProcessInput(
        faithful::SimurghManager::get_window());
    faithful::SimurghManager::get_scene()->ProcessDrawing();

    glfwSwapBuffers(faithful::SimurghManager::get_window()->Glfw());
    glfwPollEvents();
  });

  thread_pool_manager_->get_render_thread_pool()->Run();
}
*/


}  // namespace faithful
