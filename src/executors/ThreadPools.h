#ifndef FAITHFUL_THREADPOOLS_H
#define FAITHFUL_THREADPOOLS_H

#include <any>
#include <queue>
#include <thread>

#define GLFW_INCLUDE_NONE  // for arbitrary OpenGL functions including order
#include "GLFW/glfw3.h"
#include <glad/gl.h>

#include "../../utils/Function.h"
#include "../../utils/Logger.h"

namespace faithful {

class Executor;
class RenderThreadPool;
class LoadThreadPool;
class SoundThreadPool;
class ObjectThreadPool;

// if someone doesn't have any task it can steal it from other (see
// GO_threading)
enum class ExecutorType {
  GLFWThread,       // TODO: all function related to GLFW (input)
  OpenGLThread,     // TODO: all function related to OpenGL (openGL states +
                    // render_loop)
  LoadThreadPool,   // TODO: image/video/sound __init/processing__ (system
                    // file_IO + OpenCL-like processing)
  SoundThreadPool,  // TODO: music/sounds __loading__
  ObjectProcessingThreadPool  // TODO: movement/animation/ computation about
                              // collisions (what cell are we in)
};

// TODO: thread pririties

class ThreadPoolManager {
 public:
  // TODO: void Init(); <<-- thread distriburion
  ThreadPoolManager();

  RenderThreadPool* get_render_thread_pool() {
    return render_thread_pool_;
  }
  LoadThreadPool* get_load_thread_pool() {
    return load_thread_pool_;
  }
  SoundThreadPool* get_sound_thread_pool() {
    return sound_thread_pool_;
  }
  ObjectThreadPool* get_object_thread_pool() {
    return object_thread_pool_;
  }

 private:
  // TODO; RenderThreadPool --> 1xGLFW & 1xOpenGL
  RenderThreadPool* render_thread_pool_;
  LoadThreadPool* load_thread_pool_;
  SoundThreadPool* sound_thread_pool_;
  ObjectThreadPool* object_thread_pool_;
  // Executor *sound_thread_pool_;
  // Executor *object_processing_thread_pool_;
};

// IOThreadPool - __creating__ 4 threads
// max_threads = main(1) + TPInitializer(1) + GLFW(1) + OpenGL(1) +
//   OpenAl(2) + data_loading(0/2/4) + obj_processing(4/2/0) = 10
// states: NormalMode, IntensiveLoadingMode
// queues: GLFW::simple_queue, OpenGL::simple::queue, OpenAL, data_loading,
// obj_processing

// TODO: main threads (assume 6) + spare threads (assume 4) is
//    there are a lot of task in __ we take from spare ?????????? wth its shit

class ImmediateTag;

class Executor {
 public:
  using Task = folly::Function<void(void)>;

  virtual void Run() = 0;
  virtual void Put(Task task);
  /// Put(task, ImmediateTag{}) uses for urgent tasks (e.g. in RenderThreadPool)
  virtual void Put(Task task, ImmediateTag);

 protected:
  std::queue<Task>* task_queue_ = new std::queue<Task>;
};

class RenderThreadPool : public Executor {
 public:
  using Task = folly::Function<void(void)>;

  void Run() override;

  void Put(Task task, ImmediateTag) override;

  void Config(Task task);

  void AttachRenderLoop(Task task);

 private:
  Task render_loop_;
  std::queue<Task>* immediate_tasks_ = new std::queue<Task>;
  std::queue<Task>* config_tasks_ = new std::queue<Task>;
  std::thread thread_;
};

class LoadThreadPool : public Executor {
 public:
  using Task = folly::Function<void(void)>;

  void Run() override;

 private:
  std::thread thread_;
};

class SoundThreadPool : public Executor {
 public:
  void Run() override;

 private:
  std::thread thread_;
};
class ObjectThreadPool : public Executor {
 public:
  void Run() override;

 private:
  std::thread thread_;
};

}  // namespace faithful

#endif  // FAITHFUL_THREADPOOLS_H
