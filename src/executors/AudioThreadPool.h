#ifndef FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_
#define FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_

#include "Executor.h"

#include <string>
#include <type_traits>
#include <iostream> // todo: replace
#include <fstream>
#include <cstring>

#include <AL/al.h>
#include <AL/alc.h>
#include <ogg/ogg.h>

#include <vorbis/vorbisfile.h>

#include "queues/LifoBoundedMPSCBlockingQueue.h"

#include "../../config/Loader.h"

namespace faithful {
namespace details {

namespace audio {

struct StreamingAudioData {
  ALuint buffers[faithful::config::openal_buffers_num_per_music];
  std::string filename;
  std::ifstream file;
  std::uint8_t channels;
  std::int32_t sampleRate;
  std::uint8_t bitsPerSample;
  ALsizei size;
  ALuint source;
  ALsizei sizeConsumed = 0;
  ALenum format;
  OggVorbis_File oggVorbisFile;
  std::int_fast32_t oggCurrentSection = 0;
  std::size_t duration;
};

std::size_t OggReadCallback(void* ptr, std::size_t size,
                            std::size_t nmemb, void* datasource);
int OggSeekCallback(void *datasource, ogg_int64_t offset, int whence);
long OggTellCallback(void* datasource);

} // namespace audio


/** AudioThreadPool purpose:
 * - encapsulate OpenAL
 * - create & handle 6 sources:
 *   - play background music
 *   - play extra background effect#1 (e.g. weather)
 *   - play extra background effect#2 (e.g. someone speak)
 *   - 3 for sounds
 *
 * */

/// "1" - because we need only ONE openAL context
class AudioThreadPool : public StaticExecutor<1> {
 public:
  AudioThreadPool() {
    if (!InitOpenALContext()) {
      std::cerr << "Can't create OpenAL context" << std::endl;
      std::abort(); // TODO: replace by Logger::LogIF OR FAITHFUL_TERMINATE
    }
  }

  void PlayCurrent();
  void UpdateCurrent();
  void StopCurrent();

  ~AudioThreadPool() {
    alcMakeContextCurrent(nullptr);
    alcDestroyContext(openal_context_);
    alcCloseDevice(openal_device_);
  }

 private:
  bool InitOpenALContext() {
    ALCdevice* device = alcOpenDevice(nullptr);
    if (!device) {
      std::cerr << "Failed to initialize OpenAL device" << std::endl;
      return false;
    }

    ALCcontext* context = alcCreateContext(device, nullptr);
    if (!context) {
      std::cerr << "Failed to create OpenAL context" << std::endl;
      alcCloseDevice(device);
      return false;
    }

    /// we have only one OpenAL context
    alcMakeContextCurrent(context);
    return true;
  }

  ALCcontext* openal_context_;
  ALCdevice* openal_device_;
};

} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_EXECUTORS_AUDIOTHREADPOOL_H_
