#ifndef FAITHFUL_LOGGER_H
#define FAITHFUL_LOGGER_H

#include <string>
#include <fstream>
#include <cstring>
#include <fstream>
#include <iostream>
#include <type_traits>
#include <string>

#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include "Span.h"
#include "Buffer.h"


#include <AL/al.h>
#include <AL/alc.h>

//FAITHFUL_LOGGER_BUFFERING TODO:__________________________________
//FAITHFUL_LOGGER_SEVERITY_ALL
//FAITHFUL_LOGGER_SEVERITY_NO_INFO
//FAITHFUL_LOGGER_SEVERITY_ONLY_FATAL

namespace faithful {

enum class LogType {
  kInfo,
  kWarning,
  kError,
  kFatal
};

enum class LogLevel {
  kAll,
  kNoInfo,
  kOnlyFatal
};

/// There are in-game console by ~,
/// so we always writing to console (immediately in both cases):
///    in Debug: LogLevel::kAll
///    in Release: LogLevel::kNoInfo (BUT we can change it)
/// writing to file:
///    in Debug: only if explicitly __asked__; LogLevel::kAll
///              buffering/no_buffering - global namespace variable
///    in Release: kError, kWarning, kFatal
///              double buffering: waiting until first is full,
///              then switch them. If both are full we do nothing
///              Fatal errors - written only to file

/// Because of thread-safety we doesn't allow appending of log messages,
/// for example by operator "<<". To circumvent this protection you should
/// explicitly create std::string instance and (if desired) std::move() it

class Logger {
 public:
  enum class LogMode {
    kFile,
    kConsole
  };

  Logger() = default;
  Logger(LogLevel log_level)
      : log_level_(log_level) {
  }

  /// there is no need for them
  Logger(const Logger&) = delete;
  Logger(Logger&&) = delete;
  Logger& operator=(const Logger&) = delete;
  Logger& operator=(Logger&&) = delete;

  /// Problem:
  ///   we could use macros __LINE__ of __FILE__, but it's not
  ///   always convenient for __FILE__ macros cause its shows
  ///   full path to file what is redundant; concerning __LINE__:
  ///   a way more convenient not to check each single line, but
  ///   rather get info about function and exact problem.
  /// Solution:
  ///   file/line should be explicitly specified in Log message

  /// so we always writing to console (immediately in both cases):
  ///    in Debug: all types
  ///    in Release: kError, kWarning (BUT we can add command for showing kInfo)
  void Log(LogType type, std::string&& error_info) {
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
        HandleFatalError(error_info);
        break;
      default:
        break;
    }
  }

  void LogIf(LogType type, bool cond, std::string&& error_info) {
    if (cond)
      Log(type, std::move(error_info));
  }

  virtual void Flush() = 0;

  void set_log_level(LogLevel lvl) {
    log_level_ = lvl;
  }
  void set_filename(const std::string& filename) {
    filename_ = filename;
  }
  void set_filename(std::string&& filename) {
    filename_ = std::move(filename);
  }

  LogLevel get_log_level() const {
    return log_level_;
  }
  const std::string& get_filename() {
    return filename_;
  }

 protected:
  virtual void HandleFatalError(const char* error_info) {
    std::ofstream log_file(filename_, std::ofstream::out | std::ofstream::app);
    log_file << "\nFatal error: ";
    if (error_info)
      log_file.write(error_info, std::strlen(error_info));
    log_file.flush();
    log_file.close();

    // TODO: show error to screen
    std::terminate();  // TODO: not Terminate, but safe_program_exit
  }
  virtual void HandleFatalError(const std::string& error_info) {
    std::ofstream log_file(filename_, std::ofstream::out | std::ofstream::app);
    log_file << "\nFatal error: " << error_info;
    log_file.flush();
    log_file.close();

    // TODO: show error to screen
    std::terminate();  // TODO: not Terminate, but safe_program_exit
  }

  virtual void WriteMessage(std::string&& error_info,
                            const char* extra_error_info) = 0;

  constexpr static int default_message_size = 64;
  constexpr static char* default_filename = "log.txt";
#ifdef DEBUG_BUILD
  /// in Release build buffering is always turned on
  static bool buffering_on;
#endif
  LogLevel log_level_ = LogLevel::kAll;
  std::string filename_ = default_filename;
  std::atomic_flag busy_ = ATOMIC_FLAG_INIT;
};

class ConsoleLogger : public Logger {
 public:
  ConsoleLogger() = default;  // LogLevel::kAll by default
  explicit ConsoleLogger(LogLevel log_level)
      : Logger(log_level) {
  }

  void Flush() override {
    std::cout << std::flush;
  }

  void WriteMessage(std::string&& error_info,
                    const char* extra_error_info) override {
    WriteMessageImpl(std::move(error_info), extra_error_info);
  }

 private:
  /// we're using std::cout (not cerr/clog), because
  /// then we automatically std::flush output
  /// it seems we lose immediate error information, but corresponding
  /// IOThreadPool should handle it adequately in term of time
  void WriteMessageImpl(std::string&& error_info, const char* extra_error_info) {
    if (!busy_.test_and_set(std::memory_order_relaxed)) {
      auto accumulated_data = buffer_.Read();
      for (auto i : accumulated_data) {
        if (*(i.Data()) == 0)
          break;
        std::cout.write(i.Data(), i.Size()) << '\n';
      }
      std::cout.write(extra_error_info, std::strlen(extra_error_info));
      WriteConsoleMessage(error_info);

      Flush();  // TODO: its too often, call FileLogger::Flush instead
      busy_.clear();
    } else {
      utility::SpanBuffer<char> error_str;
      error_str.Write(extra_error_info);
      error_str.Write(error_info);
      buffer_.Write(error_str);
    }
  }
  void WriteConsoleMessage(const char* error_info) {
    std::cout.write(error_info, std::strlen(error_info)) << '\n';
  }
  void WriteConsoleMessage(const std::string& error_info) {
    std::cout.write(error_info.data(), error_info.length()) << '\n';
  }

  utility::SpanBufferPool<char> buffer_;
};
class FileLogger : public Logger {
 public:
  FileLogger()
      : Logger(LogLevel::kNoInfo) {
    log_file_.open(default_filename, std::ofstream::out | std::ofstream::app);
  }
  explicit FileLogger(LogLevel log_level)
      : Logger(log_level) {
    log_file_.open(default_filename, std::ofstream::out | std::ofstream::app);
  }

  ~FileLogger() {
    log_file_.close();
  }

  void Flush() override {
    log_file_.flush();
  }

  void HandleFatalError(const char* error_info) override {
    log_file_ << "\nFatal error: ";
    if (error_info)
      log_file_.write(error_info, std::strlen(error_info));
    log_file_.flush();
    log_file_.close();

    // TODO: show error to screen
    std::terminate();  // TODO: not Terminate, but safe_program_exit
  }
  void HandleFatalError(const std::string& error_info) override {
    log_file_ << "\nFatal error: " << error_info;
    log_file_.flush();
    log_file_.close();

    // TODO: show error to screen
    std::terminate();  // TODO: not Terminate, but safe_program_exit
  }

  void WriteMessage(std::string&& error_info,
                    const char* extra_error_info) override {
    WriteMessageImpl(std::move(error_info), extra_error_info);
  }

 private:
  void WriteMessageImpl(std::string&& error_info, const char* extra_error_info) {
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

  utility::SwitchSpanBufferPool<char> buffer_;
  std::ofstream log_file_;
};

#define GL_CALL(function, ...) GlCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
void CheckOpenGlError(std::string&& error_info);

#define GLFW_CALL(function, ...) GlfwCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
void CheckOpenGlfwError(std::string&& error_info);

#define AL_CALL(function, ...) AlCallImpl(__FILE__, __LINE__, function, __VA_ARGS__)
#define ALC_CALL(function, device, ...) AlcCallImpl(__FILE__, __LINE__, function, device, __VA_ARGS__)
void CheckOpenAlError(std::string&& error_info);

template<typename Fn, typename... Args>
auto GlCallImpl(const char* filename, int line, Fn fn, Args... args)
    ->typename std::enable_if<std::is_same<void, decltype(fn(args...))>::value,
                               decltype(fn(args...))>::type {
  function(std::forward<Args>(args)...);
#ifndef FAITHFUL_DEBUG
  std::string error_info;
  error_info += "Error OpenGL (";
  error_info += filename;
  error_info += ": ";
  error_info += line;
  error_info += ") ";
  CheckOpenGlError(std::move(error_info));
#endif
}

template<typename Fn, typename... Args>
auto GlfwCallImpl(const char* filename, int line, Fn fn, Args... args)
    ->typename std::enable_if<std::is_same<void, decltype(fn(args...))>::value,
                               decltype(fn(args...))>::type {
  function(std::forward<Args>(args)...);
#ifndef FAITHFUL_DEBUG
  std::string error_info;
  error_info += "Error GLFW (";
  error_info += filename;
  error_info += ": ";
  error_info += line;
  error_info += ") ";
  CheckOpenGlfwError(std::move(error_info));
#endif
}

template<typename Fn, typename... Args>
auto AlCallImpl(const char* filename, int line, Fn fn, Args... args)
    ->typename std::enable_if<std::is_same<void, decltype(fn(args...))>::value,
                               decltype(fn(args...))>::type {
  function(std::forward<Args>(args)...);
#ifndef FAITHFUL_DEBUG
  std::string error_info;
  error_info += "Error OpenAL (";
  error_info += filename;
  error_info += ": ";
  error_info += line;
  error_info += ") ";
  CheckOpenAlError(std::move(error_info));
#endif
}

template<typename Fn, typename... Args>
auto AlcCallImpl(const char* filename, int line, Fn fn, Args... args)
    ->typename std::enable_if<!std::is_same<void, decltype(fn(args...))>::value,
                               decltype(fn(args...))>::type {
#ifndef FAITHFUL_DEBUG
  auto ret = function(std::forward<Args>(args)...);
  std::string error_info;
  error_info += "Error OpenAL (";
  error_info += filename;
  error_info += ": ";
  error_info += line;
  error_info += ") ";
  CheckOpenAlError(std::move(error_info));
  return ret;
#else
  return function(std::forward<Args>(args)...);
#endif
}

#ifdef FAITHFUL_DEBUG
bool Logger::buffering_on = true;
#endif

}  // namespace faithful

#endif  // FAITHFUL_LOGGER_H
