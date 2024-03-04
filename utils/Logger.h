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

#include <AL/al.h>
#include <AL/alc.h>


/////////////////////////////////////////////
/////////////////////////////////////////////
/////////////////////////////////////////////
//// DEPRECATED //// (need to be refactored)

//FAITHFUL_LOGGER_BUFFERING TODO:__________________________________
//FAITHFUL_LOGGER_SEVERITY_ALL
//FAITHFUL_LOGGER_SEVERITY_NO_INFO
//FAITHFUL_LOGGER_SEVERITY_ONLY_FATAL

namespace faithful {


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
//bool Logger::buffering_on = true;
#endif

}  // namespace faithful

#endif  // FAITHFUL_LOGGER_H
