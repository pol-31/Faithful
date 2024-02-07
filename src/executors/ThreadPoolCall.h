#ifndef FAITHFUL_THREADPOOLCALL_H
#define FAITHFUL_THREADPOOLCALL_H

#include "../Engine.h"

#include "ThreadPools.h"
#include <iostream>  // TODO: replace by logger

namespace faithful {

// TODO: SimurghEngine should storage __ThreadInitialization_instance__
class ImmediateTag {};

// can be chained but there is no warranty of order
class SingleSemiDeferredCall {
 public:
  SingleSemiDeferredCall() {
    executor_ = CurrentRenderThreadPool();
  }
  SingleSemiDeferredCall(class Executor* executor)
      : executor_(executor) {
  }

  SingleSemiDeferredCall& Executor(ExecutorType type) {
    switch (type) {
      case ExecutorType::GLFWThread:
        executor_ = CurrentRenderThreadPool();
        break;
      case ExecutorType::OpenGLThread:
        executor_ = CurrentRenderThreadPool();
        break;
      case ExecutorType::LoadThreadPool:
        executor_ = CurrentLoadThreadPool();
        break;
      case ExecutorType::SoundThreadPool:
        executor_ = CurrentSoundThreadPool();
        break;
      case ExecutorType::ObjectProcessingThreadPool:
        executor_ = CurrentObjectThreadPool();
        break;
    }
    return *this;
  }

  template <typename Callable>
  SingleSemiDeferredCall& Do(Callable&& fn) {
    static_assert(std::is_invocable_v<Callable>, "Callable must be invocable.");
    executor_->Put([f = std::forward<Callable>(fn)]() {
      f();
    });
    return *this;
  }

  template <typename Callable, typename ResType>
  SingleSemiDeferredCall& Do(Callable&& fn, ResType& result) {
    static_assert(std::is_invocable_v<Callable>, "Callable must be invocable.");
    executor_->Put(std::move([this, f = std::forward<Callable>(fn), &result]() {
      &result = f();
    }));
    return *this;
  }

 protected:
  // TODO: NOT nullptr
  class Executor* executor_ = nullptr;
};

// full blocking, put task with the highest priority, can be chained, but
// it's the same as just consequential calls (all they are blocking)
class ImmediateCall : public SingleSemiDeferredCall {
 public:
  ImmediateCall() = default;
  ImmediateCall(class Executor* executor)
      : SingleSemiDeferredCall(executor) {
  }

  template <typename T>
  operator T() {
    while (!valid_) {
      // std::cout << "wait " << RenderThreadImpl::Empty() << std::endl;
    }
    return std::any_cast<T>(*value_);
  }

  template <typename Callable>
  typename std::enable_if<
      std::is_void<decltype(std::declval<Callable>()())>::value,
      ImmediateCall&>::type
  Do(Callable&& fn) {
    static_assert(std::is_invocable_v<Callable>, "Callable must be invocable.");
    executor_->Put((std::forward<Callable>(fn)), ImmediateTag{});
    return *this;
  }

  template <typename Callable>
  typename std::enable_if<
      !std::is_void<decltype(std::declval<Callable>()())>::value,
      ImmediateCall&>::type
  Do(Callable&& fn) {
    static_assert(std::is_invocable_v<Callable>, "Callable must be invocable.");
    executor_->Put(
        [this, f = std::forward<Callable>(fn)]() {
          this->value_ = new std::any(f());
          this->valid_ = true;
        },
        ImmediateTag{});
    return *this;
  }

 private:
  std::any* value_ = new std::any;
  bool valid_ = false;
};

class DeferredCall : public SingleSemiDeferredCall {
 public:
  DeferredCall() = default;
  DeferredCall(class Executor* executor)
      : SingleSemiDeferredCall(executor) {
  }

  ~DeferredCall() {
    std::cout << "DESTRUCTOR" << std::endl;
    if (continious_ == nullptr) {
      return;
    } else {
      executor_->Put(std::move(*continious_));
    }
  }

  template <typename Callable>
  DeferredCall& Do(Callable&& fn) {
    static_assert(std::is_invocable_v<Callable>, "Callable must be invocable.");
    if (continious_ == nullptr) {
      continious_ = new folly::Function<void(void)>(std::forward<Callable>(fn));
    } else {
      executor_->Put(std::move(*continious_));
      continious_ = new folly::Function<void(void)>(std::forward<Callable>(fn));
    }
    return *this;
  }

  template <typename Callable, typename ResType>
  DeferredCall& Do(Callable&& fn, ResType& result) {
    static_assert(std::is_invocable_v<Callable>, "Callable must be invocable.");
    if (continious_ == nullptr) {
      continious_ = new folly::Function<void(void)>(std::forward<Callable>(fn));
    } else {
      executor_->Put(std::move(*continious_));
      DeferredCall* instance = this;
      continious_ = new folly::Function<void(void)>(
          std::move([this, f = std::forward<Callable>(fn), &result]() {
            &result = f();
          }));
    }
    return *this;
  }

 private:
  folly::Function<void(void)>* continious_ = nullptr;
};

}  // namespace faithful

#endif  // FAITHFUL_THREADPOOLCALL_H
