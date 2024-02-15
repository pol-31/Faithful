#include "AssetLoadingThreadPool.h"

#include <iostream>

AssetLoadingThreadPool::AssetLoadingThreadPool(int thread_number)
    : joined_(false) {
  // subtracted by 1 because it's Main thread (see explanation in header)
  int actual_thread_number = std::max(1, thread_number) - 1;
  if (actual_thread_number == 0) {
    std::cerr << "Error: AssetLoadingThreadPool has 0 worker threads!"
              << "\nTry to increase number of threads" << std::endl;
    return;
  }
  threads_ = std::vector<std::thread>(actual_thread_number);
  threads_left_.store(0);
}

void AssetLoadingThreadPool::Run() {
  for (int i = 0; i < threads_.size(); ++i) {
    threads_[i] = std::thread([&, i](){
      while (!joined_) {
        {
          std::unique_lock lock(mu_all_ready_);
          thread_tasks_ready_.wait(lock, [this]() {
            return threads_left_.load() != 0;
          });
        }
        WorkAndWaitAll(i);
        if (joined_) {
          break;
        }
      }
    });
  }
}

void AssetLoadingThreadPool::Join() {
  joined_ = true;
  // its like poison pill but allows thread to check was thread pool joined
  // (see AssetLoadingThreadPool::Run() if (joined_) { break; })
  threads_left_.store(GetThreadNumber() - 1); // this thread don't work
  threads_task_ = [](int){};
  thread_tasks_ready_.notify_all();
  for (auto& thread : threads_) {
    thread.join();
  }
}

void AssetLoadingThreadPool::Execute(TaskType task) {
  threads_left_.store(GetThreadNumber());
  threads_task_ = std::move(task);
  thread_tasks_ready_.notify_all();

  // because threads_size() == all_threads - 1, where 1 is Calling thread
  WorkAndWaitAll(threads_.size());
}

void AssetLoadingThreadPool::WorkAndWaitAll(int thread_id) {
  threads_task_(thread_id);
  threads_left_.fetch_sub(1);
  if (threads_left_.load() == 0) {
    thread_tasks_completed_.notify_all();
  } else {
    std::unique_lock lock(mu_all_completed_);
    thread_tasks_completed_.wait(lock, [this]() {
      return threads_left_.load() == 0;
    });
  }
}
