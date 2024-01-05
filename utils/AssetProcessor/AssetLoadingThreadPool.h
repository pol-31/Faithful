#ifndef FAITHFUL_ASSETLOADINGTHREADPOOL_H
#define FAITHFUL_ASSETLOADINGTHREADPOOL_H

#include <string>
#include <stack>
#include <thread>
#include <vector>

#include <iostream>  // todo; temp

#include <atomic>
#include <mutex>
#include <condition_variable>

#include "AssetCategory.h"
#include "AssetInfo.h"

class AudioProcessor;
class ModelProcessor;
class TextureProcessor;

#include "../Function.h"

/** Processing order:
 * "full-threaded" audio
 * "full-threaded" textures
 * other audio, textures, models
 * */

/* This should provide all need-to-process paths:
 * for models - just path;
 * for texture - path, thread_count
 * for audio - path, thread_count
 *
 * so we can create two queues:
 * #1 "full-threaded" : process all assets by one
 * #2 "1-threaded" : process all assets
 *
 * AssetThreadPool: threads == std::thread::hardware...
 * assume there is 8 threads, then 8 has its own while(true)-loops,
 * where one of them (called King-thread) are allowed to check
 * two global queues: "full"/"1"-"threaded" and:
 * for full-threaded it loading data (decompresses using stb_image/dr_libs)
 *   then dividing it on 8 blocks and put to each of 7-slaves-threads
 * for 1-threaded it gives each function to take from that global queue
 *   CAS operation can have sense there
 *
 * */

/** There is two phases:
 * 1) Main (external) thread has access to single_threaded_tasks_ and
 *   for each threads_ it setting Task-s for each internal threads (threads_).
 *   tasks: multi_threaded_tasks_
 * 2) Now each threads is "independent" and can take tasks on its own.
 *   tasks: single_threaded_tasks_
 * ! phases should be switched MANUALLY by SwitchPhase()
 * */
// TODO: mb add inheritance from Faithful/utils/Executor.h
class AssetLoadingThreadPool {
 public:
  using FunctionType = folly::Function<void()>;

  AssetLoadingThreadPool();
  AssetLoadingThreadPool(int thread_count);

  /// textures are managed by Main thread
  void Run(AudioProcessor* audio_processor, ModelProcessor* model_processor);

  void PutSingleThreadedTask(AssetInfo&& task) {
    single_threaded_tasks_.push(task);
  }
  void PutMultiThreadedTask(AssetInfo&& task) {
    multi_threaded_tasks_.push(task);
  }

  int get_thread_count() {
    return thread_count_;
  }

 protected:
  friend class AssetProcessor;
  friend class TextureProcessor;
  friend class AudioProcessor;
  // for Main thread (or just thread-manager from the outside)
  std::vector<std::pair<FunctionType, bool>>& RetainTaskContainers() {
    return threads_tasks_;
  }

  void SwitchPhase() {
    single_threaded_tasks_accessor_.clear();
    first_phase = !first_phase;
  }

  void UpdateContext();
  bool Completed();

  static void ProcessSingleThreaded(AudioProcessor* audio_processor,
                                    ModelProcessor* model_processor,
                                    const AssetInfo& task);

  // shared between all threads
  // Guarded by atomic_flag multi_threaded_tasks_accessor_
  std::stack<AssetInfo> single_threaded_tasks_;

  // Should be accessed only from Main thread
  std::stack<AssetInfo> multi_threaded_tasks_;

  // Guarded by thread_tasks_mutex_
  std::vector<std::pair<FunctionType, bool>> threads_tasks_;

  std::condition_variable thread_tasks_accessor_;
  std::mutex thread_tasks_mutex_;

 private:
  void DefaultInitThreadTasks() {
    for (int i = 0; i < thread_count_; ++i) {
      threads_tasks_[i].second = false;
      threads_completion_[i] = false;
    }
  }

  std::vector<std::thread> threads_;

  // Guarded by atomic_flag threads_completion_accessor_
  std::vector<bool> threads_completion_;

  int thread_count_;

  std::atomic_flag single_threaded_tasks_accessor_ = ATOMIC_FLAG_INIT;
  std::atomic_flag threads_completion_accessor_ = ATOMIC_FLAG_INIT;

  bool first_phase = true;
};

#endif  // FAITHFUL_ASSETLOADINGTHREADPOOL_H
