#ifndef FAITHFUL_SRC_EXECUTORS_SUPPORTTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_SUPPORTTHREADPOOL_H_

#include <iostream>

#include "IExecutor.h"
#include "queues/IQueue.h"

namespace faithful {
namespace details {

/// Join() can be called multiple times
class SupportThreadPool : public IDynamicExecutor {
 public:
  SupportThreadPool() = delete;
  SupportThreadPool(queue::IQueueBase<IDynamicExecutor::TaskType>* queue) {
    task_queue_ = queue;
  }

  // TODO: SetQueue() ?

  void Run() override {
    if (state_ == IExecutor::State::kRunning) { // TODO: warning
      Join();
    }
    state_ = IExecutor::State::kRunning;
    for (auto& thread : threads_) {
      thread = std::thread{[&] {
        while (state_ != IExecutor::State::kJoined) {
          task_queue_->Take()();
        }
      }};
    }
  }

  virtual void Join() override {
    state_ = IExecutor::State::kJoined;
    for (auto& thread : threads_) {
      thread.join();
    }
    auto threads_num = threads_.size();
    threads_.clear();
    while (--threads_num > 0) {
      threads_.push_back({});
    }
  }

  virtual void SetThreadsNumber(int num) override {
    int actual_num = std::max(num, 0);
    if (actual_num == 0) {
      return;
    }
    if (state_ == IExecutor::State::kRunning) {
      if (actual_num > threads_.size()) {
        int extra_threads_num = actual_num - threads_.size();
        while (--extra_threads_num != 0) {
          threads_.push_back(std::thread{[&] {
            while (state_ != IExecutor::State::kJoined) {
              task_queue_->Take()();
            }
          }});
        }
      } else {
        // TODO: warning
        return;
      }
    } else {
      threads_.clear();
      while (--actual_num != 0) {
        threads_.push_back(std::thread{[&] {
          while (state_ != IExecutor::State::kJoined) {
            task_queue_->Take()();
          }
        }});
      }
    }
  }
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_SUPPORTTHREADPOOL_H_
