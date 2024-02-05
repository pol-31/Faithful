#include "Logger.h"

namespace faithful {

void Logger::Log(LogType type, std::string&& error_info) {
  switch (type) {
    // TODO: FAITHFUL_MAIN_LOGGER
#ifndef LOG_INFO_MESSAGES
    case LogType::kInfo:
      if (log_level_ == LogLevel::kAll)
        WriteMessage(std::move(error_info), "Info: ");
      break;
#endif
    case LogType::kWarning:
      if (log_level_ != LogLevel::kOnlyFatal)
        WriteMessage(std::move(error_info), "Warning: ");
      break;
    case LogType::kError:
      if (log_level_ != LogLevel::kOnlyFatal)
        WriteMessage(std::move(error_info), "Error: ");
      break;
    case LogType::kFatal:
      HandleFatalError(std::move(error_info));
      break;
    default:
      break;
  }
}

void Logger::HandleFatalError(std::string&& error_info) {
  std::ofstream log_file(filename_, std::ofstream::out | std::ofstream::app);
  log_file << "\nFatal error: " << error_info;
  log_file.flush();
  log_file.close();

  // TODO: show error to screen
  std::terminate();  // TODO: not Terminate, but safe_program_exit
}

#ifndef FAITHFUL_DEBUG // todo "ifdef"
static inline ConsoleLogger* global_logger = new ConsoleLogger;
#else
static inline FileLogger* global_logger = new FileLogger;
#endif

void ConsoleLogger::WriteMessage(std::string&& error_info,
                                 const char* extra_error_info) {
  if (!busy_.test_and_set(std::memory_order_relaxed)) {
    auto accumulated_data = buffer_.Read();
    for (auto i : accumulated_data) {
      if (*(i.Data()) == 0)
        break;
      std::cout.write(i.Data(), i.Size()) << '\n';
    }
    std::cout.write(extra_error_info, std::strlen(extra_error_info));
    std::cout.write(error_info.data(), error_info.length()) << '\n';

    Flush();  // TODO: its too often, call FileLogger::Flush instead
    busy_.clear();
  } else {
    utility::SpanBuffer<char> error_str;
    error_str.Write(extra_error_info);
    error_str.Write(error_info);
    buffer_.Write(error_str);
  }
}

void FileLogger::HandleFatalError(std::string&& error_info) {
  log_file_ << "\nFatal error: " << error_info;
  log_file_.flush();
  log_file_.close();

  // TODO: show error to screen
  std::terminate();  // TODO: not Terminate, but safe_program_exit
}

void FileLogger::WriteMessage(std::string&& error_info,
                              const char* extra_error_info) {
#ifdef DEBUG_BUILD
  if (!busy_.test_and_set(std::memory_order_relaxed)) {
    if ((buffering_on && buffer_.Full()) || !buffering_on) {
      auto accumulated_data = buffer_.Read();
      for (auto i : accumulated_data.first) {
        if (*(i.Data()) == 0)
          break;
        log_file_.write(i.Data(), i.Size()) << '\n';
      }
      for (auto i : accumulated_data.second) {
        if (*(i.Data()) == 0)
          break;
        log_file_.write(i.Data(), i.Size()) << '\n';
      }
      log_file_ << extra_error_info << error_info << '\n';
      Flush();  // TODO: its too often, call FileLogger::Flush instead
      busy_.clear();
      return;
    }
    busy_.clear();
  }
  utility::SpanBuffer<char> error_str;
  error_str.Write(extra_error_info);
  error_str.Write(error_info);
  buffer_.Write(error_str);
#else
  if (!busy_.test_and_set(std::memory_order_relaxed)) {
    if (buffer_.Full()) {
      auto accumulated_data = buffer_.Read();
      for (auto i : accumulated_data.first) {
        if (*(i.Data()) == 0)
          break;
        log_file_.write(i.Data(), i.Size()) << '\n';
      }
      for (auto i : accumulated_data.second) {
        if (*(i.Data()) == 0)
          break;
        log_file_.write(i.Data(), i.Size()) << '\n';
      }
      log_file_ << extra_error_info << error_info << '\n';
      Flush();  // TODO: its too often, call FileLogger::Flush instead
      busy_.clear();
      return;
    }
    busy_.clear();
  }
  utility::SpanBuffer<char> error_str;
  error_str.Write(extra_error_info);
  error_str.Write(error_info);
  buffer_.Write(error_str);
#endif
}

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
