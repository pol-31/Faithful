#include "ThreadPools.h"

#include <any>
#include <queue>
#include <thread>
#include <atomic>

#define GLFW_INCLUDE_NONE  // for arbitrary OpenGL functions including order
#include <GLFW/glfw3.h>
#include <glad/gl.h>

#include "../../src/Engine.h"
#include "../Logger.h"
#include "../Function.h"
#include "ThreadPoolCall.h"

namespace faithful {

ThreadPoolManager::ThreadPoolManager() {
  render_thread_pool_ = new RenderThreadPool;
  load_thread_pool_ = new LoadThreadPool;
  sound_thread_pool_ = new SoundThreadPool;
  object_thread_pool_ = new ObjectThreadPool;
}

/*void ThreadPoolManager::Init() {
  if (initialized_) return;
  std::size_t cores_num = std::thread::hardware_concurrency();
  if (cores_num >= 8) {
    // do smt
  } else if (cores_num >= 4) {
    // do smt
  } else if (cores_num >= 2) {
    // do smt
  } else {
    // do smt
  }
  initialized_ = true;
}*/

void RenderThreadPool::Run() {
  thread_ = std::thread([&]() {
    while (!config_tasks_->empty()) {
      config_tasks_->front()();
      config_tasks_->pop();
    }
    SimurghManager::Initialization();
    while (true) {
      while (faithful::SimurghManager::get_window() == nullptr) {
      }
      while (!glfwWindowShouldClose(
          faithful::SimurghManager::get_window()->Glfw())) {
        while (!immediate_tasks_->empty()) {
          immediate_tasks_->front()();
          immediate_tasks_->pop();
        }
        while (!task_queue_->empty()) {
          task_queue_->front()();
          task_queue_->pop();
        }
        render_loop_();
      }
      glfwTerminate();
    }
  });
  thread_.detach();
}

void Executor::Put(Task task) {
  task_queue_->push(std::move(task));
}
void Executor::Put(Task task, ImmediateTag) {
  task_queue_->push(std::move(task));
}

void RenderThreadPool::Put(Task task, ImmediateTag) {
  std::atomic_flag completed_flag = ATOMIC_FLAG_INIT;
  completed_flag.clear();
  task_queue_->push(std::move(task));
  immediate_tasks_->emplace([&completed_flag]() {
    completed_flag.test_and_set();
  });

  while (completed_flag.test_and_set(std::memory_order_acquire)) {
  }
}

void RenderThreadPool::Config(Task task) {
  config_tasks_->push(std::move(task));
}

void RenderThreadPool::AttachRenderLoop(Task task) {
  render_loop_ = std::move(task);
}

void LoadThreadPool::Run() {
  thread_ = std::thread([&]() {
    while (true) {
      while (!task_queue_->empty()) {
        task_queue_->front()();
        task_queue_->pop();
      }
    }
  });
  thread_.detach();
}
void SoundThreadPool::Run() {
  thread_ = std::thread([&]() {
    while (true) {
      while (!task_queue_->empty()) {
        task_queue_->front()();
        task_queue_->pop();
      }
    }
  });
  thread_.detach();
}
void ObjectThreadPool::Run() {
  thread_ = std::thread([&]() {
    while (true) {
      while (!task_queue_->empty()) {
        task_queue_->front()();
        task_queue_->pop();
      }
    }
  });
  thread_.detach();
}

}  // namespace faithful
