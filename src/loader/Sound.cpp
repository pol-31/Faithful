#include "Sound.h"

#include "../executors/AudioThreadPool.h"

namespace faithful {
void Sound::Play() {
  data_->audio_thread_pool->Play(*this);
}

} // namespace faithful
