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
  void Log(LogType type, std::string&& error_info);

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
  virtual void HandleFatalError(std::string&& error_info);

  virtual void WriteMessage(std::string&& error_info,
                            const char* extra_error_info) = 0;

  constexpr static int default_message_size = 64; // TODO: ----------------------- config/Loader.h
  constexpr static char* default_filename = "log.txt"; // TODO: ----------------------- config/Loader.h
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

  /// we're using std::cout (not cerr/clog), because
  /// then we automatically std::flush output
  /// it seems we lose immediate error information, but corresponding
  /// IOThreadPool should handle it adequately in term of time
  void WriteMessage(std::string&& error_info,
                    const char* extra_error_info) override;

 private:

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

  void HandleFatalError(std::string&& error_info) override;

  void WriteMessage(std::string&& error_info,
                    const char* extra_error_info) override;

 private:
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

std::string GenErrorString(const char* meta, const char* filename, int line);

template<typename Fn, typename... Args>
auto GlCallImpl(const char* filename, int line, Fn fn, Args... args)
    ->typename std::enable_if<std::is_same<void, decltype(fn(args...))>::value,
                               decltype(fn(args...))>::type {
  fn(std::forward<Args>(args)...);
#ifndef FAITHFUL_DEBUG
  std::string error_info = GenErrorString("OpenGL", filename, line);
  CheckOpenGlError(std::move(error_info));
#endif
}

template<typename Fn, typename... Args>
auto GlfwCallImpl(const char* filename, int line, Fn fn, Args... args)
    ->typename std::enable_if<std::is_same<void, decltype(fn(args...))>::value,
                               decltype(fn(args...))>::type {
  fn(std::forward<Args>(args)...);
#ifndef FAITHFUL_DEBUG
  std::string error_info = GenErrorString("GLFW", filename, line);
  CheckOpenGlfwError(std::move(error_info));
#endif
}

template<typename Fn, typename... Args>
auto AlCallImpl(const char* filename, int line, Fn fn, Args... args)
    ->typename std::enable_if<std::is_same<void, decltype(fn(args...))>::value,
                               decltype(fn(args...))>::type {
  fn(std::forward<Args>(args)...);
#ifndef FAITHFUL_DEBUG
  std::string error_info = GenErrorString("OpenAL", filename, line);
  CheckOpenAlError(std::move(error_info));
#endif
}

template<typename Fn, typename... Args>
auto AlcCallImpl(const char* filename, int line, Fn fn, Args... args)
    ->typename std::enable_if<!std::is_same<void, decltype(fn(args...))>::value,
                               decltype(fn(args...))>::type {
#ifndef FAITHFUL_DEBUG
  auto ret = fn(std::forward<Args>(args)...);
  std::string error_info = GenErrorString("OpenAL", filename, line);
  CheckOpenAlError(std::move(error_info));
  return ret;
#else
  return fn(std::forward<Args>(args)...);
#endif
}

#ifdef FAITHFUL_DEBUG // TODO: --------------------------------------------------< wtf is this
bool Logger::buffering_on = true;
#endif

}  // namespace faithful

#endif  // FAITHFUL_LOGGER_H
