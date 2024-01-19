#include "AssetLoadingThreadPool.h"

#include <iostream>

#include "AudioProcessor.h"
#include "TextureProcessor.h"
#include "ModelProcessor.h"

AssetLoadingThreadPool::AssetLoadingThreadPool() {
  // subtracted by 1 because there we need only parallelism,
  // "not concurrency" and one thread is already running main thread
  thread_count_ = std::thread::hardware_concurrency() - 1;
  threads_ = std::vector<std::thread>(thread_count_);
  threads_tasks_ = std::vector<std::pair<FunctionType, bool>>(thread_count_);
  threads_completion_ = std::vector<bool>(thread_count_);
  DefaultInitThreadTasks();
}

/// recommendation: thread_count not including Main thread
AssetLoadingThreadPool::AssetLoadingThreadPool(int thread_count) {
  thread_count_ = std::max(1, thread_count);
  threads_ = std::vector<std::thread>(thread_count_);
  threads_tasks_ = std::vector<std::pair<FunctionType, bool>>(thread_count_);
  threads_completion_ = std::vector<bool>(thread_count_);
  DefaultInitThreadTasks();
}

void AssetLoadingThreadPool::Run(AudioProcessor* audio_processor,
                                 ModelProcessor* model_processor) {
  std::cout << single_threaded_tasks_.size() << std::endl;
  std::cout << multi_threaded_tasks_.size() << std::endl;
  for (int i = 0; i < thread_count_; ++i) {
    threads_[i] = std::thread([&, id = i, task = &threads_tasks_[i]]() {
      /// first_phase should be changed by SwitchPhases() from
      /// (external to thread pool) thread - like Main thread
      while (first_phase) {
        while (!thread_tasks_mutex_.try_lock()) {
        }
        thread_tasks_mutex_.unlock();
        if (task->second) {
          (task->first)();
          task->second = false;
          while (threads_completion_accessor_.test_and_set()) {
          }
          threads_completion_[id] = true;  // TODO: memory order
          threads_completion_accessor_.clear();
        }
      }
      // now only single-threaded tasks left
      while (true) {
        while (single_threaded_tasks_accessor_.test_and_set()) {
        }
        if (!single_threaded_tasks_.empty()) {
          ProcessSingleThreaded(audio_processor, model_processor,
                                single_threaded_tasks_.top());
          single_threaded_tasks_.pop();
        } else {
          single_threaded_tasks_accessor_.clear();  // TODO: memory order
          break;
        }
        single_threaded_tasks_accessor_.clear();  // TODO: memory order
      }

      while (threads_completion_accessor_.test_and_set()) {
      }
      threads_completion_[id] = true;  // TODO: memory order
      threads_completion_accessor_.clear();
    });
    threads_[i].detach();
  }
}

void AssetLoadingThreadPool::UpdateContext() {
  while (threads_completion_accessor_.test_and_set()) {
  }
  for (std::size_t i = 0; i < threads_completion_.size(); ++i)
    threads_completion_[i] = false;  // TODO: memory order
  threads_completion_accessor_.clear();
}

bool AssetLoadingThreadPool::Completed() {
  bool result = true;
  for (int i = 0; i < thread_count_; ++i) {
    result &= threads_completion_[i];
  }
  return result;
}

void AssetLoadingThreadPool::ProcessSingleThreaded(
    AudioProcessor* audio_processor, ModelProcessor* model_processor,
    const AssetInfo& task) {
  if (task.category == AssetCategory::kAudioSound) {
    audio_processor->ProcessSound(task.source_path, task.relative_path);
  } else {  // AsserCategory::kModel
    model_processor->Process(task.source_path, task.relative_path);
  }
  // other tasks are full-threaded and have been already processed
}
