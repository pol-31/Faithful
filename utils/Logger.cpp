#include "Logger.h"


/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
//// DEPRECATED //// (need to be refactored)

namespace faithful {

void CheckOpenGlError(std::string&& error_info) {
  GLenum error = glGetError();
  switch (error) {
    case GL_NO_ERROR:
      return;
    case GL_INVALID_ENUM:
      global_logger->WriteMessage(std::move(error_info), "GL: invalid enum");
      return;
    case GL_INVALID_VALUE:
      global_logger->WriteMessage(std::move(error_info), "GL: invalid value");
      return;
    case GL_INVALID_OPERATION:
      global_logger->WriteMessage(std::move(error_info), "GL: invalid operation");
      return;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      global_logger->WriteMessage(std::move(error_info), "GL: invalid framebuffer operation");
      return;
    case GL_OUT_OF_MEMORY:
      global_logger->WriteMessage(std::move(error_info), "GL: out of memory");
      return;
    default:
      global_logger->WriteMessage(std::move(error_info), "GL: undefined error");
      return;
  }
}

void CheckOpenGlfwError(std::string&& error_info) {
  int error = glfwGetError(nullptr);
  switch (error) {
    case GLFW_NO_ERROR:
      return;
    case GLFW_NOT_INITIALIZED:
      global_logger->WriteMessage(std::move(error_info), "GLFW: not initialized");
      return;
    case GLFW_NO_CURRENT_CONTEXT:
      global_logger->WriteMessage(std::move(error_info), "GLFW: no current context");
      return;
    case GLFW_INVALID_ENUM:
      global_logger->WriteMessage(std::move(error_info), "GLFW: invalid enum");
      return;
    case GLFW_INVALID_VALUE:
      global_logger->WriteMessage(std::move(error_info), "GLFW: invalid value");
      return;
    case GLFW_OUT_OF_MEMORY:
      global_logger->WriteMessage(std::move(error_info), "GLFW: out of memory");
      return;
    case GLFW_API_UNAVAILABLE:
      global_logger->WriteMessage(std::move(error_info), "GLFW: api unavailable");
      return;
    case GLFW_VERSION_UNAVAILABLE:
      global_logger->WriteMessage(std::move(error_info), "GLFW: version unavailable");
      return;
    case GLFW_PLATFORM_ERROR:
      global_logger->WriteMessage(std::move(error_info), "GLFW: platform error");
      return;
    case GLFW_FORMAT_UNAVAILABLE:
      global_logger->WriteMessage(std::move(error_info), "GLFW: format unavailable");
      return;
    case GLFW_NO_WINDOW_CONTEXT:
      global_logger->WriteMessage(std::move(error_info), "GLFW: no window context");
      return;
    default:
      global_logger->WriteMessage(std::move(error_info), "GLFW: undefined error");
      return;
  }
}

void CheckOpenAlError(std::string&& error_info) {
  GLenum error = alGetError();
  switch (error) {
    case GL_NO_ERROR:
      return;
    case GL_INVALID_ENUM:
      global_logger->WriteMessage(std::move(error_info), "AL: invalid name");
      return;
    case GL_INVALID_VALUE:
      global_logger->WriteMessage(std::move(error_info), "AL: invalid enum");
      return;
    case GL_INVALID_OPERATION:
      global_logger->WriteMessage(std::move(error_info), "AL: invalid value");
      return;
    case GL_INVALID_FRAMEBUFFER_OPERATION:
      global_logger->WriteMessage(std::move(error_info), "AL: out of memory");
      return;
    case GL_OUT_OF_MEMORY:
      global_logger->WriteMessage(std::move(error_info), "AL: invalid operation");
      return;
    default:
      global_logger->WriteMessage(std::move(error_info), "AL: undefined error");
      return;
  }
}


std::string GenErrorString(const char* meta, const char* filename, int line) {
  std::string error_string;
  error_string += "Error ";
  error_string += meta;
  error_string += " (";
  error_string += filename;
  error_string += ": ";
  error_string += std::to_string(line);
  error_string += ") ";
  return std::move(error_string);
}

} // namespace faithful
