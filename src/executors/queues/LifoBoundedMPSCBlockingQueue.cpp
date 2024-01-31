#include "LifoBoundedMPSCBlockingQueue.h"

namespace faithful {
namespace details {
namespace queue {

void LifoBoundedMPSCBlockingQueue::Pop() {
  // blocking pop (?)
}
void LifoBoundedMPSCBlockingQueue::Push(Task&&) {
  // blocking push
}

} // namespace queue
} // namespace details
} // namespace faithful