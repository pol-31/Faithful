#include "Window.h"


#define GLFW_INCLUDE_NONE // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>


#include <iostream>

namespace faithful {

Window::Window() {
  glm::vec2 window_size = CalculateDefaultResolution();
  CreateDefaultGlfwWindow("Simurgh project", window_size.x, window_size.y);
}

Window::Window(std::string_view title) {
  // create window (0.5*width ; 0.5*height)
  glm::vec2 window_size = CalculateDefaultResolution();
  CreateDefaultGlfwWindow(title, window_size.x, window_size.y);
}
Window::Window(std::size_t width) {
  CreateDefaultGlfwWindow("Simurgh project", width, width);
}
Window::Window(std::string_view title, std::size_t width) {
  CreateDefaultGlfwWindow(title, width, width);
}
Window::Window(std::size_t width, std::size_t height) {
  CreateDefaultGlfwWindow("Simurgh project", width, height);
}
Window::Window(std::string_view title, std::size_t width,
               std::size_t height) {
  CreateDefaultGlfwWindow(title, width, height);
}


void Window::CreateDefaultGlfwWindow(
    std::string_view title,
    std::size_t width,
    std::size_t height) {

  // TODO: more default hints _________________________________<<

  glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
  glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
  glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

#ifdef __APPLE__
  glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

  GLFWwindow *window =
    glfwCreateWindow(width, height, "Faithful", nullptr, nullptr);
  //simurgh::Logger::LogIf(simurgh::LogType::kFatal, !window)
  //  <<"Unable to create GLFW window";
  glfwMakeContextCurrent(window);
  window_ = window;

  AttachSizeCallback(DefaultSizeCallback);
  // TODO: callbacks
}

glm::vec2 Window::CalculateDefaultResolution() {
  glm::vec2 resolution = MonitorResolution();
  return glm::vec2(resolution.x, resolution.y);
}

glm::vec2 Window::MonitorResolution() {
  resolution_ = glm::vec2(1600, 800);
  monitors_.CurrentMonitorResolution();
  return resolution_;
}

Window::~Window() {
  glfwDestroyWindow(window_);
}

void Window::Close() {
  glfwSetWindowShouldClose(window_, true);
}

void Window::FullscreenOn() {
  Monitor::Mode mode = monitors_.Current()->CurMode();
  glfwSetWindowMonitor(window_, monitors_.Current()->Glfw(), 0, 0,
                       mode.width, mode.height, mode.framerate);
}

void Window::FullscreenOff() {
  glfwSetWindowMonitor(window_, nullptr, 0, 0,
                       resolution_.x, resolution_.y, 0);
}

void DefaultSizeCallback(GLFWwindow* window __attribute__((unused)),
                         int width, int height) {
  glViewport(0, 0, width, height);
}


} // namespace faithful
