#ifndef FAITHFUL_SRC_LOADER_ASSETS_DATA_SOUNDDATA_H_
#define FAITHFUL_SRC_LOADER_ASSETS_DATA_SOUNDDATA_H_

#include <string>
#include <fstream>
#include <memory>

#include <AL/al.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace faithful {
namespace details {

class AudioThreadPool;

namespace assets {

struct SoundData {
  std::string filename;
  int channels;
  int sample_rate;
  int bits_per_sample;
  ALsizei size;
  ALenum format;
  std::unique_ptr<char> data;
  AudioThreadPool* audio_thread_pool;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ASSETS_DATA_SOUNDDATA_H_
