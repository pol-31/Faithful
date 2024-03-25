#ifndef FAITHFUL_ASSETLOADINGTHREADPOOL_H
#define FAITHFUL_ASSETLOADINGTHREADPOOL_H

#include <thread>
#include <vector>

#include <atomic>
#include <mutex>
#include <condition_variable>

#include <../utils/Function.h>

/// This is "blocking" thread pool what means it blocks thread from which
/// Execute() was called. So for AssetLoadingThreadPool(8) it
/// generates only 7 threads, but inside the Execute() it utilizes all 8,
/// because of caller thread

/// can be Run() and Stop() multiple times (useful for testing,
/// when you want to encode and then decode)
class AssetLoadingThreadPool {
 public:
  /// int param for thread_id
  using TaskType = folly::Function<void(int)>;

  AssetLoadingThreadPool(int thread_number = std::thread::hardware_concurrency());

  /// neither copyable nor movable because of std::mutex members
  AssetLoadingThreadPool(const AssetLoadingThreadPool& other) = delete;
  AssetLoadingThreadPool& operator=(const AssetLoadingThreadPool& other) = delete;
  AssetLoadingThreadPool(AssetLoadingThreadPool&& other) = delete;
  AssetLoadingThreadPool& operator=(AssetLoadingThreadPool&& other) = delete;

  void Run();
  void Stop();

  /// SOLELY memory_order_seq_cst
  /// (we don't spinning and need latest relevant info)

  void Execute(TaskType task);

  int GetThreadNumber() {
    return threads_.size() + 1;
  }

 private:
  void WorkAndWaitAll(int thread_id);

  TaskType threads_task_;

  /// the difference between "ready" and "complete":
  /// ready - all are ready for the next task
  /// completed - all have completed their tasks

  std::condition_variable thread_tasks_ready_;
  /// last thread who see threads_left_ == 0 -> notify_all()
  std::condition_variable thread_tasks_completed_;

  std::mutex mu_all_ready_;
  std::mutex mu_all_completed_;

  std::vector<std::thread> threads_;

  /// it has two usages:
  /// 1) condition (threads_left_ == 0) in thread_tasks_ready_ waiting
  ///    as a Predicate stop_waiting
  /// 2) counter for how many thread hasn't done task yet, so
  ///    if it's not equal to zero - some threads are still working
  int threads_left_;
  bool stopped_;
};

#endif  // FAITHFUL_ASSETLOADINGTHREADPOOL_H
