#ifndef FAITHFUL_SRC_LOADER_AUDIODATA_H_
#define FAITHFUL_SRC_LOADER_AUDIODATA_H_

#include <string>
#include <fstream>

#include <AL/al.h>
#include <vorbis/vorbisfile.h>

namespace faithful {
namespace details {

struct SoundData {
  std::string filename;
  std::uint8_t channels;
  std::int32_t sampleRate;
  std::uint8_t bitsPerSample;
  ALsizei size;
  ALenum format;
  std::size_t duration;
};

struct MusicData : public SoundData {
  std::ifstream file;
  ALsizei sizeConsumed = 0;
  OggVorbis_File oggVorbisFile;
  int oggCurrentSection = 0;
};

}  // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_AUDIODATA_H_
