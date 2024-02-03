#ifndef FAITHFUL_SRC_LOADER_AUDIODATA_H_
#define FAITHFUL_SRC_LOADER_AUDIODATA_H_

#include <string>
#include <fstream>

#include <AL/al.h>
#include <vorbis/vorbisfile.h>

namespace faithful {
namespace details {

/** We've separated SoundData from Sound.h / Music.h to avoid data
 * duplication and also avoid inheritance from Music to Sound classes
 * (we don't need such relationships between them - there would be
 * only overhead due to overriding / hiding)
 * */

struct AudioDataBase {
  std::string filename;
  int channels;
  int sample_rate;
  int bits_per_sample;
  ALsizei size = 0; // explicitly specifying uninitialized data
  ALenum format;
};

struct SoundData : public AudioDataBase {
  std::shared_ptr<char> data;
};

struct MusicData : public AudioDataBase {
  std::ifstream fstream;
  ALsizei size_consumed = 0;
  OggVorbis_File ogg_vorbis_file;
  int ogg_cur_section = 0;
};

}  // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_AUDIODATA_H_
