#ifndef FAITHFUL_SRC_LOADER_MUSIC_H_
#define FAITHFUL_SRC_LOADER_MUSIC_H_

#include "AssetBase.h"
#include "assets_data/MusicData.h"

namespace faithful {

class Music
    : public details::assets::AssetBase<details::assets::MusicData> {
 public:
  using Base = details::assets::AssetBase<details::assets::MusicData>;
  using Base::Base;
  using Base::operator=;

  const std::string& GetFilename() const {
    return data_->filename;
  }
  int GetChannels() const {
    return data_->channels;
  }
  int GetSampleRate() const {
    return data_->sample_rate;
  }
  int GetBitsPerSample() const {
    return data_->bits_per_sample;
  }
  ALsizei GetSize() const {
    return data_->size;
  }
  ALenum GetFormat() const {
    return data_->format;
  }

  const std::ifstream& GetFstream() const {
    return data_->fstream;
  }
  ALsizei GetSizeConsumed() const {
    return data_->size_consumed;
  }

  // TODO; non const ref ? yes
  OggVorbis_File& GetOggVorbisFile() {
    return data_->ogg_vorbis_file;
  }
  int& GetOggCurSection() {
    return data_->ogg_cur_section;
  }

 private:
  using Base::data_;
};

} // namespace faithful

#endif  // FAITHFUL_SRC_LOADER_MUSIC_H_
