#include "ThreadPoolCall.h"

#include "../Engine.h"

namespace faithful {

SingleSemiDeferredCall::SingleSemiDeferredCall() {
  executor_ = CurrentRenderThreadPool();
}

SingleSemiDeferredCall& SingleSemiDeferredCall::Executor(ExecutorType type) {
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

}  // namespace faithful
