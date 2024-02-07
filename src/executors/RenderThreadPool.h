#ifndef FAITHFUL_SRC_EXECUTORS_QUEUES_RENDERTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_QUEUES_RENDERTHREADPOOL_H_

#include "Executor.h"

#include <iostream> // todo: replace


#include "queues/LifoBoundedMPSCBlockingQueue.h"

#include "../../config/Loader.h"

namespace faithful {
namespace details {


/** AudioThreadPool purpose:
 * - encapsulate OpenAL
 * - create & handle 6 sources:
 *   - play background music
 *   - play extra background effect#1 (e.g. weather)
 *   - play extra background effect#2 (e.g. someone speak)
 *   - 3 for sounds
 *
 * */
 /// Neither StaticThreadPool nor DynamicThreadPool
 /// This should be run from Main Thread and its blocking (after Run())
class RenderThreadPool : public Executor {
 public:
  using Base = StaticExecutor<1>;

  RenderThreadPool();
  ~RenderThreadPool();

  void Run() override;
  void Join() override;

 private:
  void ReleaseSources();

  void InitOpenGLContext();
  void DeinitOpenGLContext();

  bool openal_initialized_ = false;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_QUEUES_RENDERTHREADPOOL_H_
