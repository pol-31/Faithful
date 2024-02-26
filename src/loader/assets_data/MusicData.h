#ifndef FAITHFUL_SRC_LOADER_ASSETS_DATA_MUSICDATA_H_
#define FAITHFUL_SRC_LOADER_ASSETS_DATA_MUSICDATA_H_

#include <string>
#include <fstream>

#include <AL/al.h>
#include <vorbis/codec.h>
#include <vorbis/vorbisfile.h>

namespace faithful {
namespace details {
namespace assets {

struct MusicData {
  std::string filename;
  int channels;
  int sample_rate;
  int bits_per_sample;
  ALsizei size;
  ALenum format;
  std::ifstream fstream;
  ALsizei size_consumed = 0;
  OggVorbis_File ogg_vorbis_file;
  int ogg_cur_section = 0;
};

} // namespace assets
} // namespace details
} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_ASSETS_DATA_MUSICDATA_H_
